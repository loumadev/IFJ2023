#include <stdlib.h>

#ifndef ARRAY_H
#define ARRAY_H

typedef struct Array {
	void **data;
	size_t size;
	size_t capacity;
} Array;

void Array_constructor(Array *array, size_t capacity);
void Array_destructor(Array *array);
void Array_push(Array *array, void *value);
void* Array_pop(Array *array);
void* Array_get(Array *array, int index);
void Array_set(Array *array, int index, void *value);
void Array_clear(Array *array);
void Array_resize(Array *array, size_t capacity);
void Array_reserve(Array *array, size_t capacity);
void Array_remove(Array *array, int index);

void Array_print_compact(Array *array);
void Array_print(Array *array, unsigned int depth, int isProperty);

Array* Array_alloc(size_t size);
void Array_free(Array *array);

#endif
