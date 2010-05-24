#ifndef _PMALLOC_H_
#define _PMALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif

void *p_malloc(int);
void *p_calloc(int elems, int size);
void p_free(void *);
void *p_realloc(void *, int newsize);

char *p_strdup(const char *s);

#ifdef __cplusplus
}
#endif

#ifdef WIN32
#define snprintf _snprintf
#endif

#ifdef SIMULATOR
#define bal_printf printf
#include <stdio.h>
#endif

#include <stdlib.h>

#endif
