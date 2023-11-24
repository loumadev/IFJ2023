#include <stdlib.h>

#ifndef ARRAY_H
#define ARRAY_H

typedef struct Array {
	void **data;
	size_t size;
	size_t capacity;
} Array;

/**
 * Initializes an array.
 *
 * @param array The array to initialize.
 * @param capacity Initial capacity of the array. (must be > 0)
 */
void Array_constructor(Array *array, size_t capacity);

/**
 * Reset array state and deallocates the memory used by the given array (not the elements!).
 *
 * @param array The array to deallocate.
 */
void Array_destructor(Array *array);

/**
 * Adds a new element to the beginning of the array.
 *
 * @param array The array to add the element to.
 * @param value The value to add to the array.
 */
void Array_unshift(Array *array, void *value);

/**
 * Removes and returns the first element of the array.
 *
 * @param array The array to remove the element from.
 * @return A pointer to the removed element, or NULL if the array is empty.
 */
void* Array_shift(Array *array);

/**
 * Adds a new element to the end of the array.
 *
 * @param array The array to add the element to.
 * @param value The value to add to the array.
 */
void Array_push(Array *array, void *value);

/**
 * Removes and returns the last element of the array.
 *
 * @param array The array to remove the element from.
 * @return A pointer to the removed element, or NULL if the array is empty.
 */
void* Array_pop(Array *array);

/**
 * Returns the element at the given index in the array.
 *
 * @param array The array to get the element from.
 * @param index The index of the element to get.
 * @return A pointer to the element at the given index, or NULL if the index is out of bounds.
 */
void* Array_get(Array *array, int index);

/**
 * Sets the value of the element at the given index in the array.
 *
 * @param array The array to set the element in.
 * @param index The index of the element to set.
 * @param value The new value to set for the element.
 */
void Array_set(Array *array, int index, void *value);

/**
 * Inserts the given value at the given index in the array.
 *
 * @param array The array to insert the element in.
 * @param index The index of the element where to insert the new element.
 * @param value The new value to insert into the array.
 */
void Array_insert(Array *array, int index, void *value);

/**
 * Removes all elements from the array.
 *
 * This function sets the size of the array to 0, but does not deallocate the memory used by the array.
 *
 * @param array The array to clear.
 */
void Array_clear(Array *array);

/**
 * Resizes the array to the given capacity.
 *
 * If the new capacity is smaller than the current size of the array, the elements at the end of the array are truncated.
 * If the new capacity is larger than the current size of the array, the new elements are initialized to NULL.
 *
 * @param array The array to resize.
 * @param capacity The new capacity of the array.
 */
void Array_resize(Array *array, size_t capacity);

/**
 * Reserves memory for the given capacity in the array.
 *
 * If the array does not have enough capacity, this function resizes the array to the given capacity.
 *
 * @param array The array to reserve memory for.
 * @param capacity The new capacity of the array.
 */
void Array_reserve(Array *array, size_t capacity);

/**
 * Removes the element at the given index from the array and shifts the remaining elements to the left.
 *
 * @param array The array to remove the element from.
 * @param index The index of the element to remove.
 * @return A pointer to the removed element, or NULL if the index is out of bounds.
 */
void* Array_remove(Array *array, int index);

/**
 * Slices the array from the given start index to the given end index.
 *
 * @param array The array to slice.
 * @param start The start index of the slice.
 * @param end The end index of the slice.
 * @return A pointer to the newly allocated array containing the slice.
 */
Array* Array_slice(Array *array, int start, int end);

/**
 * Splices the array from the given start index to the given end index.
 *
 * @param array The array to splice.
 * @param start The start index of the splice.
 * @param end The end index of the splice.
 * @return A pointer to the newly allocated array containing the splice.
 */
Array* Array_splice(Array *array, int start, int end);

/**
 * Creates a new array instance and pushes the given values to it.
 *
 * @param count Number of values to push to the array.
 * @param ... The values to push to the array.
 * @return A pointer to the newly allocated array.
 */
Array* Array_fromArgs(int count, ...);


/**
 * Prints value of the array in a compact format (content of the array).
 *
 * @param array The array to print.
 */
void Array_print_compact(Array *array);

/**
 * Prints value of the array (including properties).
 *
 * @param array The array to print.
 * @param depth The current depth of the printing. (default: 0) (used internally)
 * @param isProperty Whether the array is a property of a larger object. (default: 0) (used internally)
 */
void Array_print(Array *array, unsigned int depth, int isProperty);

/**
 * Allocates memory for a new array.
 *
 * @param size The size of the array to allocate.
 * @return A pointer to the newly allocated array.
 */
Array* Array_alloc(size_t size);

/**
 * Deallocates the memory used by the given array.
 *
 * @param array The array to deallocate.
 */
void Array_free(Array *array);

#endif
