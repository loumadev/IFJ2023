#include <stdint.h>
#include <stddef.h>
#include "internal/String.h"
#include "internal/Array.h"
#include "inspector.h"

#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_DEFAULT_CAPACITY 30
#define HASHMAP_LOAD_FACTOR 0.75
#define HASHMAP_RESIZE_FACTOR 2

typedef struct HashMapEntry {
	String *key;
	void *value;
	struct HashMapEntry *next;
} HashMapEntry;

typedef struct HashMap {
	size_t size;
	size_t capacity;
	struct HashMapEntry **entries;
} HashMap;

void HashMap_constructor(HashMap *map);
void HashMap_destructor(HashMap *map);
void HashMap_set(HashMap *map, char *key, void *value);
void* HashMap_get(HashMap *map, char *key);
bool HashMap_has(HashMap *map, char *key);
void HashMap_remove(HashMap *map, char *key);
void HashMap_clear(HashMap *map);
Array* HashMap_keys(HashMap *map);
Array* HashMap_values(HashMap *map);
void HashMap_forEach(HashMap *map, void (*callback)(String *key, void *value));

HashMap* HashMap_alloc();
void HashMap_free(HashMap *map);

void HashMap_print_compact(HashMap *map);
void HashMap_print(HashMap *map, unsigned int depth, int isProperty);

#endif
