#include "p_malloc.h"

#include "Platform.h"
#include "Debug.h"

#include <string.h>
#include <stdio.h>
#include <map>

typedef struct alloc_t {
    int size;
    const char *filename;
    int line;
    const char *function;
} alloc_t;

static std::map<void *, alloc_t> Allocations;
static int totalSize = 0;
typedef std::map<void *, alloc_t>::iterator AllocationsIterator;
static Semaphore *mallocSemaphore = NULL;

extern "C" char *p_strdup_i(const char *s, const char *filename, int line,
        const char *function)
{
	int len;
	char *output;

	len = strlen(s);
	output = (char *)p_malloc_i(len+1, filename, line, function);
	memcpy(output, s, len+1);
	return output;
}

extern "C" void *p_malloc_i(int size, const char *filename, int line,
        const char *function)
{
    void *output;

	//return malloc(size);

	if (size == 153600) {
		emo_printf("potential" NL);
	}

	if (mallocSemaphore == NULL) {
		mallocSemaphore = semaphore_create(1);
	}

	semaphoreP(mallocSemaphore);

    output = malloc(size);
    if (output != NULL) {
        Allocations[output].size = size;
        Allocations[output].filename = filename;
        Allocations[output].line = line;
        Allocations[output].function = function;
        totalSize += size;
    }

	semaphoreV(mallocSemaphore);

    return output;
}

extern "C" void *p_calloc_i(int elem, int size, const char *filename, int line,
        const char *function)
{
	void *p = p_malloc_i(size * elem, filename, line, function);
	if(!p) return NULL;
	memset(p, 0, size * elem);
	return p;
}

extern "C" void p_free_i(void *p, const char *filename, int line,
        const char *function)
{
    AllocationsIterator iter;

	//free(p);
	//return;

	semaphoreP(mallocSemaphore);

    iter = Allocations.find(p);
    if (iter == Allocations.end()) {
        emo_printf("Freeing already freed block %s:%d:%s" NL,
                filename, line, function);
        abort();
    }
	free(p);
    totalSize -= (*iter).second.size;
    Allocations.erase(iter);

	semaphoreV(mallocSemaphore);
}

extern "C" void *p_realloc_i(void *p, int s, const char *filename, int line,
        const char *function)
{
    AllocationsIterator iter;
    void *output;

	//return realloc(p, s);

	semaphoreP(mallocSemaphore);

    iter = Allocations.find(p);
    if (iter == Allocations.end() && p != NULL) {
        emo_printf("Realloc already freed block" NL);
        abort();
    }
	if (p != NULL) {
		totalSize -= (*iter).second.size;
		Allocations.erase(iter);
	}

	output = realloc(p, s);

    Allocations[output].size = s;
    Allocations[output].filename = filename;
    Allocations[output].line = line;
    Allocations[output].function = function;
    totalSize += s;

	semaphoreV(mallocSemaphore);

	return output;
}

extern "C" int p_getTotalAllocated(void)
{
	return totalSize;
}