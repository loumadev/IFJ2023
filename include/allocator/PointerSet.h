/**
 * @file include/allocator/PointerSet.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdbool.h>

#define ALLOCATOR_SET_SIZE 2048

typedef struct PointerNode {
	void *ptr;
	struct PointerNode *next;
} PointerNode;

typedef struct PointerSet {
	PointerNode *nodes[ALLOCATOR_SET_SIZE];
} PointerSet;


/**
 * Adds a pointer to the set.
 * @param set Set to add pointer to
 * @param ptr Pointer to add
 */
void PointerSet_add(PointerSet *set, void *ptr);

/**
 * Checks if the set contains a pointer.
 * @param set Set to check
 * @param ptr Pointer to check
 * @return true if the set contains the pointer, false otherwise
 */
bool PointerSet_has(PointerSet *set, void *ptr);

/**
 * Removes a pointer from the set.
 * @param set Set to remove pointer from
 * @param ptr Pointer to remove
 */
void PointerSet_remove(PointerSet *set, void *ptr);

/**
 * Clears all pointers from the set.
 * @param set Set to clear
 */
void PointerSet_clear(PointerSet *set);

/**
 * Allocates a new PointerSet.
 * @return Newly allocated PointerSet instance
 */
PointerSet* PointerSet_alloc();

/**
 * Frees a PointerSet.
 * @param set Set to free
 */
void PointerSet_free(PointerSet *set);

/** End of file include/allocator/PointerSet.h **/
