#ifndef _PMALLOC_H_
#define _PMALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SIMULATOR

void *p_malloc_i(int s, const char *filename, int line,
        const char *function);
void p_free_i(void *, const char *filename, int line,
        const char *function);
void *p_realloc_i(void *, int newsize, const char *filename, int line,
        const char *function);
void *p_calloc_i(int elem, int size, const char *filename, int line,
        const char *function);
char *p_strdup_i(const char *s, const char *filename, int line,
        const char *function);
#define p_malloc(s) p_malloc_i(s, __FILE__, __LINE__, __FUNCTION__)
#define p_free(s) p_free_i(s, __FILE__, __LINE__, __FUNCTION__)
#define p_realloc(s, n) p_realloc_i(s, n, __FILE__, __LINE__, __FUNCTION__)
#define p_calloc(s, n) p_calloc_i(s, n, __FILE__, __LINE__, __FUNCTION__)
#define p_strdup(s) p_strdup_i(s, __FILE__, __LINE__, __FUNCTION__)
#else
void *p_malloc(int);
void p_free(void *);
void *p_realloc(void *, int newsize);
void *p_calloc(int elems, int size);
char *p_strdup(const char *s);
#endif

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
