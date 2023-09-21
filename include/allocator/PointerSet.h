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