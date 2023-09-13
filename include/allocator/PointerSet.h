#include <stdbool.h>

#define ALLOCATOR_SET_SIZE 100

typedef struct Node {
	void *ptr;
	struct Node *next;
} Node;

typedef struct PointerSet {
	Node *nodes[ALLOCATOR_SET_SIZE];
} PointerSet;

void PointerSet_add(PointerSet *set, void *ptr);
bool PointerSet_has(PointerSet *set, void *ptr);
void PointerSet_remove(PointerSet *set, void *ptr);
void PointerSet_clear(PointerSet *set);

PointerSet* PointerSet_alloc();
void PointerSet_free(PointerSet *set);