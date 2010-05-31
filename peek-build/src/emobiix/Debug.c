#include "Debug.h"
#include "stdarg.h"

void emo_printf(const char *fmt, ...) {
#ifndef SIMULATOR
        va_list ap;
        va_start(ap, fmt);
        bal_vprintf(fmt, ap);
        /*bal_printf("\n");*/
        va_end(ap);
        NU_Sleep(1);
#endif
}
