#include "p_malloc.h"

#include <string.h>
#include <stdio.h>

#ifdef SIMULATOR
#define BalMalloc malloc
#define BalFree free
#define bgetr realloc
#else
#include "balapi.h"
#include "bget.h"
#endif

char *p_strdup(const char *s)
{
	int len;
	char *output;

	len = strlen(s);
	output = (char *)p_malloc(len+1);
	memcpy(output, s, len+1);
	return output;
}

void *p_malloc(int size)
{
	return BalMalloc(size);
}

void *p_calloc(int elem, int size)
{
	void *p = BalMalloc(size * elem);
	if(!p) return NULL;
	memset(p, 0, size * elem);
	return p;
}

void p_free(void *p)
{
	BalFree(p);
}

void *p_realloc(void *p, int s)
{
	return bgetr(p, s);
}
