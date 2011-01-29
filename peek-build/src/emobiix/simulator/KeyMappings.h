#ifndef _KEYMAPPINGS_H_
#define _KEYMAPPINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EKEY_ACTIVATE		13
#define EKEY_BACK			12
#define EKEY_FOCUSPREV		0xFFFFFF01
#define EKEY_FOCUSNEXT		0xFFFFFF02
#define EKEY_FOCUSLEFT		0xFFFFFF03
#define EKEY_FOCUSRIGHT		0xFFFFFF04
#define EKEY_ALTTAB			42

int MapKeyToInternal(int key);

#ifdef __cplusplus
}
#endif

#endif
