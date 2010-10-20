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

	EMO_ASSERT_NULL(output != NULL, "strdup malloc failed");

	memcpy(output, s, len+1);
	return output;
}

void *p_malloc(int size)
{
	void *p = malloc(size);
	
	EMO_ASSERT_NULL(p != NULL, "malloc failed");

	return p;
}

void *p_calloc(int elem, int size)
{
	void *p = calloc(elem, size);

	EMO_ASSERT_NULL(p != NULL, "calloc failed");

	return p;
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

void *p_realloc(void *ptr, int s)
{
	void *p = realloc(ptr, s);

	EMO_ASSERT_NULL(p != NULL, "realloc failed");

	return p;
}

