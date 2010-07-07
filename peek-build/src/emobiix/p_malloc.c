#include "p_malloc.h"

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
	return malloc(size);
}

void *p_calloc(int elem, int size)
{
	void *p = malloc(size * elem);
	if(!p) return NULL;
	memset(p, 0, size * elem);
	return p;
}

void p_free(void *p)
{
	free(p);
}

void *p_realloc(void *p, int s)
{
	return realloc(p, s);
}
