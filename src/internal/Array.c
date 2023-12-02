/**
 * @file src/internal/Array.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "internal/Array.h"

#include <stdarg.h>

#include "allocator/MemoryAllocator.h"
#include "inspector.h"

void Array_constructor(Array *array, size_t capacity) {
	if(!array) return;

	array->size = 0;
	array->data = NULL;
	Array_resize(array, capacity);
}

void Array_destructor(Array *array) {
	if(!array) return;

	if(array->data) mem_free(array->data);

	array->size = 0;
	array->data = NULL;
	array->capacity = 0;
}

void Array_unshift(Array *array, void *value) {
	if(!array) return;

	// If size exceeds capacity, resize the array to fit more elements
	if(array->size >= array->capacity) {
		Array_resize(array, (array->capacity ? array->capacity : 1) << 1);
	}

	// Shift all elements one to the right
	for(size_t i = array->size; i > 0; i--) {
		array->data[i] = array->data[i - 1];
	}

	array->data[0] = value;
	array->size++;
}

void* Array_shift(Array *array) {
	if(!array) return NULL;
	if(array->size == 0) return NULL;

	void *value = array->data[0];

	// Shift all elements one to the left
	for(size_t i = 0; i < array->size - 1; i++) {
		array->data[i] = array->data[i + 1];
	}

	array->size--;

	// If size is less than a quarter of capacity, resize the array to save memory
	if(array->size <= array->capacity >> 2) {
		Array_resize(array, array->capacity >> 1);
	}

	return value;
}

void Array_push(Array *array, void *value) {
	if(!array) return;
	// If size exceeds capacity, resize the array to fit more elements
	if(array->size >= array->capacity) {
		Array_resize(array, (array->capacity ? array->capacity : 1) << 1);
	}

	array->data[array->size++] = value;
}

void* Array_pop(Array *array) {
	if(!array) return NULL;
	if(array->size == 0) return NULL;

	void *value = array->data[--array->size];

	// If size is less than a quarter of capacity, resize the array to save memory
	if(array->size <= array->capacity >> 2) {
		Array_resize(array, array->capacity >> 1);
	}

	return value;
}

int __Array_resolveIndex(Array *array, int index) {
	if(!array) return 0;
	if(index < 0) return array->size + index;
	return index;
}

void* Array_get(Array *array, int index) {
	if(!array) return NULL;

	index = __Array_resolveIndex(array, index);
	if((size_t)index >= array->size) return NULL;

	return array->data[index];
}

void Array_set(Array *array, int index, void *value) {
	if(!array) return;

	index = __Array_resolveIndex(array, index);

	size_t size = index + 1;

	// If there's not enough capacity to fit the index, resize the array
	if(size > array->capacity) {
		Array_resize(array, size);
	}

	// If the index is past the end of the array, set the size to the index
	if(size > array->size) {
		array->size = size;
	}

	array->data[index] = value;
}

void Array_insert(Array *array, int index, void *value) {
	if(!array) return;

	index = __Array_resolveIndex(array, index);

	// If the target index is past the end of the array, just set it
	if((size_t)index >= array->size) {
		Array_set(array, index, value);
		return;
	}

	Array_reserve(array, array->size + 1);

	// Shift all elements after index, one to the right
	for(size_t i = array->size; i > (size_t)index; i--) {
		array->data[i] = array->data[i - 1];
	}

	array->data[index] = value;
	array->size++;
}

void Array_clear(Array *array) {
	if(!array) return;

	array->size = 0;
	Array_resize(array, 0);
}

void Array_resize(Array *array, size_t capacity) {
	if(!array) return;

	if(capacity) {
		// Non-zero capacity => reallocate the array
		array->data = mem_recalloc(array->data, array->capacity, capacity, sizeof(void*));
	} else {
		// Zero capacity => free the array and set it to NULL
		if(array->data) mem_free(array->data);
		array->data = NULL;
	}

	array->capacity = capacity;
}

void Array_reserve(Array *array, size_t capacity) {
	if(!array) return;

	if(array->capacity < capacity) {
		Array_resize(array, capacity);
	}
}

void* Array_remove(Array *array, int index) {
	if(!array) return NULL;

	index = __Array_resolveIndex(array, index);
	if((size_t)index >= array->size) return NULL;

	void *value = array->data[index];

	// Shift all elements after index, one to the left
	for(size_t i = index; i < array->size - 1; i++) {
		array->data[i] = array->data[i + 1];
	}

	array->size--;

	// Resize the array if necessary to save memory
	if(array->size <= array->capacity >> 2) {
		Array_resize(array, array->capacity >> 1);
	}

	return value;
}

Array* Array_slice(Array *array, int start, int end) {
	if(!array) return NULL;

	start = __Array_resolveIndex(array, start);
	end = __Array_resolveIndex(array, end);

	if(start < 0) start = 0;
	if((size_t)end > array->size) end = array->size;

	// Return an empty array if there is nothing to extract
	if(end <= start) return Array_alloc(0);

	Array *newArray = Array_alloc(end - start);

	for(int i = start; i < end; i++) {
		Array_push(newArray, array->data[i]);
	}

	return newArray;
}

Array* Array_splice(Array *array, int start, int end) {
	if(!array) return NULL;

	start = __Array_resolveIndex(array, start);
	end = __Array_resolveIndex(array, end);

	if(start < 0) start = 0;
	if((size_t)end > array->size) end = array->size;

	// Return an empty array if there is nothing to extract
	if(end <= start) return Array_alloc(0);

	Array *newArray = Array_alloc(end - start);

	for(int i = start; i < end; i++) {
		Array_push(newArray, array->data[i]);
	}

	// Shift all elements after end, to the left
	for(size_t i = end; i < array->size; i++) {
		array->data[i - (end - start)] = array->data[i];
	}

	array->size -= end - start;

	// Resize the array if necessary to save memory
	if(array->size <= array->capacity >> 2) {
		Array_resize(array, array->capacity >> 1);
	}

	return newArray;
}

Array* Array_fromArgs(int count, ...) {
	Array *array = Array_alloc(count);
	if(!array) return NULL;

	va_list args;
	va_start(args, count);

	for(int i = 0; i < count; i++) {
		void *value = va_arg(args, void*);
		Array_push(array, value);
	}

	va_end(args);

	return array;
}


Array* Array_alloc(size_t size) {
	Array *array = mem_alloc(sizeof(Array));
	if(!array) return NULL;

	Array_constructor(array, size);
	return array;
}

void Array_free(Array *array) {
	if(!array) return;

	Array_destructor(array);
	mem_free(array);
}

void Array_print_compact(Array *array) {
	if(!array) return (void)print_null_nl();

	const size_t size = array->size;
	const int multiline = size > 4;

	print_type_head("Array", "[");
	if(multiline) putchar('\n');
	for(size_t i = 0; i < size; i++) {
		if(multiline) indent(1);
		void *value = array->data[i];

		if(!value) print_null();
		else print_pointer(value);
		if(i != size - 1) print_separator();
		if(multiline) putchar('\n');

	}
	print_type_tail("]");
}

void Array_print(Array *array, unsigned int depth, int isProperty) {
	if(!array) {
		print_null_type("Array");
		return;
	}

	print_type_begin("Array");

	print_field("data");
	print_obj_begin(1);
	for(size_t i = 0; i < array->size; i++) {
		void *value = array->data[i];

		print_formatted_field("%zu", i);
		if(!value) print_null_field();
		else print_pointer_field(value);
	}
	print_obj_end();

	print_field("size", "%zu", array->size);

	print_field("capacity", "%zu", array->capacity);

	print_type_end();
}

/** End of file src/internal/Array.c **/
