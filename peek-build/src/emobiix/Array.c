#include "Array.h"

#include "Debug.h"

#include "p_malloc.h"

#include <stddef.h>
#include <string.h>

struct Array_t {
    void *data;
    size_t size;
    size_t length;
    int elementSize;
    float factor;
};

Array *array_new(size_t elementSize)
{
    Array *output;

	EMO_ASSERT_NULL(elementSize != 0,
			"Array created with 0 element size")

    output = (Array *)p_malloc(sizeof(Array));
    output->data = NULL;
    output->size = 0;
    output->length = 0;
    output->elementSize = elementSize;
    output->factor = 2.2f;

    return output;
}

Array *array_newWithSize(size_t elementSize, size_t size)
{
    Array *output;

    output = array_new(elementSize);
    array_resize(output, size);

    return output;
}

void array_setFactor(Array *a, float factor)
{
	EMO_ASSERT(a != NULL,
			"Setting array factor on NULL array")

    a->factor = factor;
}

void array_resize(Array *a, size_t newSize)
{
	EMO_ASSERT(a != NULL,
			"Resizing array on NULL array")

    a->data = (void *)p_realloc(a->data, newSize*a->elementSize);
    a->size = newSize;
    if (a->length > newSize)
        a->length = newSize;
}

int array_size(Array *a)
{
	EMO_ASSERT_INT(a != NULL, 0,
			"Getting array size on NULL array")

    return a->size;
}

int array_length(Array *a)
{
	EMO_ASSERT_INT(a != NULL, 0,
			"Getting array length on NULL array")

    return a->length;
}

void array_shrink(Array *a)
{
	EMO_ASSERT(a != NULL,
			"Shrinking array on NULL array")

    array_resize(a, a->length);
}

void array_append(Array *a, void *data)
{
    size_t newSize;

	EMO_ASSERT(a != NULL, "Appending Data to NULL array")

    if (a->size == a->length) {
        newSize = a->size + 1;
        newSize = (size_t)((newSize * a->factor) + 0.5f);
        array_resize(a, newSize);
    }
	++a->length;
    array_setItem(a, a->length-1, data);
}

void array_delete(Array *a)
{
	EMO_ASSERT(a != NULL,
			"Deleting NULL array")

	p_free(a->data);
	p_free(a);
}

void *array_item(Array *a, size_t index)
{
	EMO_ASSERT_NULL(a != NULL,
			"Getting array item on NULL array")
	EMO_ASSERT_NULL(index < a->length,
			"Getting array item at index beyond array length")

    return (void *)(((char *)a->data)+index*a->elementSize);
}

void array_setItem(Array *a, size_t index, void *data)
{
	EMO_ASSERT(a != NULL,
			"Setting array item on NULL array")
	EMO_ASSERT(index < a->length,
			"Setting array item at index beyond array length")

    memcpy(((char *)a->data)+a->elementSize*index, data, a->elementSize);
}

void array_removeItem(Array *a, size_t index)
{
	EMO_ASSERT(a != NULL,
			"Removing array item on NULL array")
	EMO_ASSERT(index < a->length,
			"Removing array item at index beyond array length")

    memcpy((void *)(((const char *)a->data)+index*a->elementSize),
            (void *)(((const char *)a->data)+(index+1)*a->elementSize),
            (a->length-1-index)*a->elementSize);
}

void array_empty(Array *a)
{
	EMO_ASSERT(a != NULL,
			"Attempting to empty a NULL array")
	a->length = 0;
}
