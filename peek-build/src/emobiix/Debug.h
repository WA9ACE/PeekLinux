#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifndef SIMULATOR
#include "balapi.h"
#endif

#define EMO_DEBUG_UI  (0)
#define EMO_DLVL_WARN (1)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIMULATOR
void emo_printf(const char *fmt, ...);
#else
#define emo_printf printf
#endif

#ifdef __cplusplus
}
#endif
#endif /* _DEBUG_H_ */
