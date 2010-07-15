#include "p_malloc.h"
#ifndef SIMULATOR
#include "bget.h"
#endif
#include <string.h>
#include <stdio.h>

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
#ifdef SIMULATOR
	return malloc(size);
#else
	return bget(size);
#endif
}

void *p_calloc(int elem, int size)
{
	void *p = p_malloc(size * elem);
	if(!p) return NULL;
	memset(p, 0, size * elem);
	return p;
}

void p_free(void *p)
{
#ifdef SIMULATOR
	free(p);
#else
	brel(p);
#endif
}

void *p_realloc(void *p, int s)
{
#ifdef SIMULATOR
	return realloc(p, s);
#else
	return bgetr(p,s);
#endif
}
