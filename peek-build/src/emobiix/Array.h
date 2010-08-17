#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Array_t;
typedef struct Array_t Array;

Array *array_new(size_t elementSize);
Array *array_newWithSize(size_t elementSize, size_t size);
void array_setFactor(Array *a, float factor);
void array_resize(Array *a, size_t newSize);
int array_size(Array *a);
int array_length(Array *a);
void array_shrink(Array *a);
void array_append(Array *a, void *data);
void array_delete(Array *a);
void array_empty(Array *a);
void *array_item(Array *a, size_t index);
void array_setItem(Array *a, size_t index, void *data);
void array_removeItem(Array *a, size_t index);

#ifdef __cplusplus
}
#endif

#endif /* _ARRAY_H_ */
