#include "Array.h"

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

    output = (Array *)p_malloc(sizeof(Array));
    output->data = NULL;
    output->size = 0;
    output->length = 0;
    output->elementSize = elementSize;
    output->factor = 2.2f;

    return output;
}

Array *array_new_with_size(size_t elementSize, size_t size)
{
    Array *output;

    output = array_new(elementSize);
    array_resize(output, size);

    return output;
}

void array_set_factor(Array *a, float factor)
{
    a->factor = factor;
}

void array_resize(Array *a, size_t newSize)
{
    a->data = (void *)p_realloc(a->data, newSize*a->elementSize);
    a->size = newSize;
    if (a->length > newSize)
        a->length = newSize;
}

int array_size(Array *a)
{
    return a->size;
}

int array_length(Array *a)
{
    return a->length;
}

void array_shrink(Array *a)
{
    array_resize(a, a->length);
}

void array_append(Array *a, void *data)
{
    size_t newSize;

    if (a->size == a->length) {
        newSize = a->size + 1;
        newSize = (size_t)((newSize * a->factor) + 0.5f);
        array_resize(a, newSize);
    }
    array_setItem(a, a->length, data);
    ++a->length;
}

void array_delete(Array *a)
{
	p_free(a->data);
	p_free(a);
}

void **array_item(Array *a, size_t index)
{
    return (void *)(((char *)a->data)+index*a->elementSize);
}

void array_setItem(Array *a, size_t index, void *data)
{
    memcpy(((char *)a->data)+a->elementSize*index, &data, a->elementSize);
}

void array_removeItem(Array *a, size_t index)
{
    memcpy((void *)(((const char *)a->data)+index*a->elementSize),
            (void *)(((const char *)a->data)+(index+1)*a->elementSize),
            (a->length-1-index)*a->elementSize);
}

