#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assertf.h"

// #define ALLOCATOR_USE_DEFAULT

enum AllocatorAction {
	MEMORY_ALLOC,
	MEMORY_CALLOC,
	MEMORY_REALLOC,
	MEMORY_FREE,
	MEMORY_CLEANUP
};

void* mem_alloc(size_t size);
void* mem_calloc(size_t nitems, size_t size);
void* mem_realloc(void *ptr, size_t size);
void mem_free(void *ptr);

void* safe_malloc(size_t size);
void* safe_calloc(size_t nitems, size_t size);
void* safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);

void Allocator_cleanup();