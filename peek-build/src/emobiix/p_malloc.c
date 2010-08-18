#include "p_malloc.h"
#include "Debug.h"
#include <string.h>
#include <stdio.h>

char *p_strdup(const char *s)
{
	int len;
	char *output;

	EMO_ASSERT_NULL(s != NULL, "strdup on NULL string")

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
	return malloc(size);
#endif
}

void *p_calloc(int elem, int size)
{
	return calloc(elem, size);
/*
	void *p = p_malloc(size * elem);
	if(!p) return NULL;
	memset(p, 0, size * elem);
	return p;
*/
}

void p_free(void *p)
{
#ifdef SIMULATOR
	free(p);
#else
	if(!p){ 
	    //emo_printf("______________FREE TRIED TO FREE A NULL POINTER....________________");
	    return;
	}
	free(p);
#endif
}

void *p_realloc(void *p, int s)
{
#ifdef SIMULATOR
	return realloc(p, s);
#else
	return realloc(p,s);
#endif
}
