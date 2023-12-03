/**
 * @file include/allocator/MemoryAllocator.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assertf.h"

// #define ALLOCATOR_USE_DEFAULT

enum AllocatorAction {
	MEMORY_ALLOC,
	MEMORY_CALLOC,
	MEMORY_REALLOC,
	MEMORY_RECALLOC,
	MEMORY_FREE,
	MEMORY_CLEANUP
};


/**
 * Allocates `size` bytes of memory and returns a pointer to the allocated memory.
 * Guaranteed to return a valid pointer. Uses currently selected allocator.
 * @param size Size of memory to allocate
 * @return void*
 */
void* mem_alloc(size_t size);

/**
 * Allocates and zero-initializes memory of `nitems` elements of `size` bytes
 * each and returns a pointer to the allocated memory.
 * Guaranteed to return a valid pointer. Uses currently selected allocator.
 * @param nitems Number of elements to allocate
 * @param size Size of each element
 * @return void*
 */
void* mem_calloc(size_t nitems, size_t size);

/**
 * Changes the size of the memory block pointed to by `ptr` to `size` bytes
 * and returns a pointer to the newly allocated memory.
 * Guaranteed to return a valid pointer. Uses currently selected allocator.
 * @param ptr Pointer to memory to reallocate
 * @param size New size of memory
 * @return void*
 */
void* mem_realloc(void *ptr, size_t size);

/**
 * Changes the size of the memory block pointed to by `ptr` to `nitems` elements
 * of `size` bytes each and returns a pointer to the newly allocated memory. Newly
 * allocated memory is zero-initialized.
 * Guaranteed to return a valid pointer. Uses currently selected allocator.
 * @param ptr Pointer to memory to reallocate
 * @param oldNitems Old number of elements
 * @param nitems New number of elements
 * @param size Size of each element
 * @return void*
 */
void* mem_recalloc(void *ptr, size_t oldNitems, size_t nitems, size_t size);

/**
 * Changes the size of the memory block pointed to by `ptr` to `nitems` elements
 * of `size` bytes each and returns a pointer to the newly allocated memory. Newly
 * allocated memory is zero-initialized.
 * Guaranteed to return a valid pointer. Uses currently selected allocator.
 * @param ptr Pointer to free
 */
void mem_free(void *ptr);

/**
 * Allocates `size` bytes of memory and returns a pointer to the allocated memory.
 * Guaranteed to return a valid pointer. Uses default allocator.
 * @param size Size of memory to allocate
 * @return void*
 */
void* safe_malloc(size_t size);

/**
 * Allocates and zero-initializes memory of `nitems` elements of `size` bytes
 * each and returns a pointer to the allocated memory.
 * Guaranteed to return a valid pointer. Uses default allocator.
 * @param nitems Number of elements to allocate
 * @param size Size of each element
 * @return void*
 */
void* safe_calloc(size_t nitems, size_t size);

/**
 * Changes the size of the memory block pointed to by `ptr` to `size` bytes
 * and returns a pointer to the newly allocated memory.
 * Guaranteed to return a valid pointer. Uses default allocator.
 * @param ptr Pointer to memory to reallocate
 * @param size New size of memory
 * @return void*
 */
void* safe_realloc(void *ptr, size_t size);

/**
 * Changes the size of the memory block pointed to by `ptr` to `nitems` elements
 * of `size` bytes each and returns a pointer to the newly allocated memory. Newly
 * allocated memory is zero-initialized.
 * Guaranteed to return a valid pointer. Uses default allocator.
 * @param ptr Pointer to free
 */
void safe_free(void *ptr);

/**
 * Frees all the memory from memory pool allocated by custom allocator.
 */
void Allocator_cleanup();

/** End of file include/allocator/MemoryAllocator.h **/
