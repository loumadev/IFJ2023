#include "allocator/MemoryAllocator.h"
#include "allocator/PointerSet.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int PointerSet_hash(void *ptr) {
	return (uint64_t)ptr % ALLOCATOR_SET_SIZE;
}

void PointerSet_add(PointerSet *pointerSet, void *ptr) {
	if(!pointerSet) return;

	uint32_t hash = PointerSet_hash(ptr);
	PointerNode *node = safe_malloc(sizeof(PointerNode));

	node->ptr = ptr;
	node->next = pointerSet->nodes[hash]; // Prepend
	pointerSet->nodes[hash] = node;
}

bool PointerSet_has(PointerSet *pointerSet, void *ptr) {
	if(!pointerSet) return false;

	uint32_t hash = PointerSet_hash(ptr);
	PointerNode *node = pointerSet->nodes[hash];

	while(node) {
		if(node->ptr == ptr) return true;
		node = node->next;
	}

	return false;
}

void PointerSet_remove(PointerSet *pointerSet, void *ptr) {
	if(!pointerSet) return;

	uint32_t hash = PointerSet_hash(ptr);
	PointerNode *node = pointerSet->nodes[hash];

	if(!node) return;

	if(node->ptr == ptr) {
		pointerSet->nodes[hash] = node->next;
		safe_free(node);
		return;
	}

	while(node->next) {
		if(node->next->ptr == ptr) {
			PointerNode *next = node->next->next;
			safe_free(node->next);
			node->next = next;
			return;
		}

		node = node->next;
	}
}

void PointerSet_clear(PointerSet *pointerSet) {
	if(!pointerSet) return;

	for(size_t i = 0; i < ALLOCATOR_SET_SIZE; i++) {
		PointerNode *node = pointerSet->nodes[i];
		if(!node) continue;

		while(node) {
			PointerNode *next = node->next;
			safe_free(node->ptr);
			safe_free(node);
			node = next;
		}

		pointerSet->nodes[i] = NULL;
	}
}

PointerSet* PointerSet_alloc() {
	PointerSet *pointerSet = safe_malloc(sizeof(PointerSet));
	memset(pointerSet, 0, sizeof(PointerSet));
	return pointerSet;
}

void PointerSet_free(PointerSet *pointerSet) {
	if(!pointerSet) return;
	PointerSet_clear(pointerSet);
	safe_free(pointerSet);
}