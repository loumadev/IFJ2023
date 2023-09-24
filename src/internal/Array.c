#include "internal/Array.h"
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

void Array_push(Array *array, void *value) {
	if(!array) return;

	// If size exceeds capacity, resize the array to fit more elements
	if(array->size >= array->capacity) {
		Array_resize(array, array->capacity << 1);
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
	if((size_t)index >= array->size) return;

	array->data[index] = value;
}

void Array_clear(Array *array) {
	if(!array) return;

	array->size = 0;
	Array_resize(array, 0);
}

void Array_resize(Array *array, size_t capacity) {
	if(!array) return;

	array->capacity = capacity;

	if(capacity) {
		// Non-zero capacity => reallocate the array
		array->data = mem_realloc(array->data, array->capacity * sizeof(void*));
	} else {
		// Zero capacity => free the array and set it to NULL
		if(array->data) mem_free(array->data);
		array->data = NULL;
	}
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
