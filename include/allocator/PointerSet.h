/**
 * @file include/allocator/PointerSet.h
 * @author Author Name <xlogin00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdbool.h>

#define ALLOCATOR_SET_SIZE 100

typedef struct PointerNode {
	void *ptr;
	struct PointerNode *next;
} PointerNode;

typedef struct PointerSet {
	PointerNode *nodes[ALLOCATOR_SET_SIZE];
} PointerSet;

void PointerSet_add(PointerSet *set, void *ptr);
bool PointerSet_has(PointerSet *set, void *ptr);
void PointerSet_remove(PointerSet *set, void *ptr);
void PointerSet_clear(PointerSet *set);

PointerSet* PointerSet_alloc();
void PointerSet_free(PointerSet *set);

/** End of file include/allocator/PointerSet.h **/
