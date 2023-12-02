/**
 * @file src/allocator/MemoryAllocator.c
 * @author Author Name <xlogin00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "assertf.h"
#include "allocator/MemoryAllocator.h"
#include "allocator/PointerSet.h"
#include "compiler/Result.h"

#define PREFIX "[Allocator] "

// Private
void Allocator_handleFailure() {
	fprintf(stderr, "Memory allocation failed!\n");
	exit(RESULT_ERROR_INTERNAL);
}

// Private
void* Allocator_validatePointer(void *ptr) {
	if(ptr) return ptr;

	Allocator_handleFailure();
	return NULL;
}


void* safe_malloc(size_t size) {
	return Allocator_validatePointer(malloc(size));
}

void* safe_calloc(size_t nitems, size_t size) {
	return Allocator_validatePointer(calloc(nitems, size));
}

void* safe_realloc(void *ptr, size_t size) {
	return Allocator_validatePointer(realloc(ptr, size));
}

void safe_free(void *ptr) {
	free(ptr);
}


// Private
void* Allocator_memoryAction(void *ptr, size_t oldNitems, size_t nitems, size_t size, enum AllocatorAction action) {
	// Use static variable to avoid globals (has the same effect as a global variable tho, but not mentioned in the instructions ;) )
	static PointerSet *set = NULL;

	// Create a set if it doesn't exist yet
	if(!set) set = PointerSet_alloc();

	switch(action) {
		case MEMORY_ALLOC: {
			assertf(size > 0, PREFIX "malloc: Cannot allocate 0 bytes");

			#ifdef ALLOCATOR_USE_DEFAULT
			return safe_malloc(size);
			#endif

			// Allocate memory
			void *ptr = safe_malloc(size);

			// Add the pointer to the set
			PointerSet_add(set, ptr);

			// Return the pointer
			return ptr;
		} break;

		case MEMORY_CALLOC: {
			assertf(nitems > 0, PREFIX "calloc: Cannot allocate 0 items");
			assertf(size > 0, PREFIX "calloc: Cannot allocate 0 bytes");

			#ifdef ALLOCATOR_USE_DEFAULT
			return safe_calloc(nitems, size);
			#endif

			// Allocate memory
			void *ptr = safe_calloc(nitems, size);

			// Add the pointer to the set
			PointerSet_add(set, ptr);

			// Return the pointer
			return ptr;
		} break;

		case MEMORY_REALLOC: {
			// assert(ptr != NULL); // realloc with NULL pointer acts like a malloc
			assertf(size > 0, PREFIX "realloc: Cannot allocate 0 bytes");

			#ifdef ALLOCATOR_USE_DEFAULT
			return safe_realloc(ptr, size);
			#endif

			ptr && assertf(PointerSet_has(set, ptr), PREFIX "realloc: Provided pointer has not been allocated by this allocator (Maybe used 'malloc()' instead of 'mem_alloc()'?)");

			// Remove the pointer from the set
			PointerSet_remove(set, ptr);

			// Reallocate memory
			void *newPtr = safe_realloc(ptr, size);

			// Add the pointer to the set
			PointerSet_add(set, newPtr);

			// Return the new pointer
			return newPtr;
		} break;

		case MEMORY_RECALLOC: {
			// assert(ptr != NULL); // realloc with NULL pointer acts like a malloc
			assertf(size > 0, PREFIX "recalloc: Cannot allocate 0 bytes");

			#ifdef ALLOCATOR_USE_DEFAULT
			fassertf(PREFIX "recalloc: There's no recalloc implementation in the default allocator");
			#endif

			ptr && assertf(PointerSet_has(set, ptr), PREFIX "recalloc: Provided pointer has not been allocated by this allocator (Maybe used 'malloc()' instead of 'mem_alloc()'?)");

			// Remove the pointer from the set
			PointerSet_remove(set, ptr);

			// Allocate new zero-initialized memory
			void *newPtr = safe_calloc(nitems, size);

			// Copy the data when the old pointer is not NULL
			if(ptr) {
				// Copy the data from the old pointer to the new one
				memcpy(newPtr, ptr, (oldNitems > nitems ? nitems : oldNitems) * size);

				// Free the old pointer
				safe_free(ptr);
			}

			// Add the pointer to the set
			PointerSet_add(set, newPtr);

			// Return the new pointer
			return newPtr;
		} break;

		case MEMORY_FREE: {
			assertf(ptr != NULL, PREFIX "free: Cannot free NULL pointer");

			#ifdef ALLOCATOR_USE_DEFAULT
			safe_free(ptr);
			return NULL;
			#endif

			assertf(PointerSet_has(set, ptr), PREFIX "free: Provided pointer has not been allocated by this allocator (Maybe used 'malloc()' instead of 'mem_alloc()'?)");

			// Remove the pointer from the set
			PointerSet_remove(set, ptr);

			// Free the pointer
			safe_free(ptr);
		} break;

		case MEMORY_CLEANUP: {
			#ifdef ALLOCATOR_USE_DEFAULT
			return NULL;
			#endif

			PointerSet_clear(set); // Clearing set will free all pointers
			PointerSet_free(set);
			set = NULL;
		} break;
	}

	return NULL;
}


void Allocator_cleanup() {
	Allocator_memoryAction(NULL, 0, 0, 0, MEMORY_CLEANUP);
}

void* mem_alloc(size_t size) {
	return Allocator_memoryAction(NULL, 0, 0, size, MEMORY_ALLOC);
}

void* mem_calloc(size_t nitems, size_t size) {
	return Allocator_memoryAction(NULL, 0, nitems, size, MEMORY_CALLOC);
}

void* mem_realloc(void *ptr, size_t size) {
	return Allocator_memoryAction(ptr, 0, 0, size, MEMORY_REALLOC);
}

void* mem_recalloc(void *ptr, size_t oldNitems, size_t nitems, size_t size) {
	return Allocator_memoryAction(ptr, oldNitems, nitems, size, MEMORY_RECALLOC);
}

void mem_free(void *ptr) {
	Allocator_memoryAction(ptr, 0, 0, 0, MEMORY_FREE);
}

#undef PREFIX

/** End of file src/allocator/MemoryAllocator.c **/
