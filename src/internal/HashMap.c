#include <stdio.h>

#include "internal/HashMap.h"
#include "allocator/MemoryAllocator.h"
#include "utils.h"

void HashMap_constructor(HashMap *map) {
	if(!map) return;

	map->size = 0;
	map->capacity = HASHMAP_DEFAULT_CAPACITY;
	map->entries = mem_calloc(map->capacity, sizeof(HashMapEntry*));
}

void HashMap_destructor(HashMap *map) {
	if(!map) return;

	for(size_t i = 0; i < map->size; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			HashMapEntry *next = entry->next;
			String_free(entry->key);
			mem_free(entry);
			entry = next;
		}
	}

	mem_free(map->entries);
	map->entries = NULL;
	map->capacity = 0;
	map->size = 0;
}

// Private
uint32_t HashMap_hash(char *key) {
	uint32_t hash = 0;

	for(size_t i = 0; i < strlen(key); i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

// Private
void HashMap_resize(HashMap *map, size_t capacity) {
	if(!map) return;

	// Create a new array of entries
	HashMapEntry **entries = mem_calloc(capacity, sizeof(HashMapEntry*));
	if(!entries) return;

	// Rehash all entries
	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		// Rehash all entries in the bucket
		while(entry) {
			HashMapEntry *next = entry->next;
			uint32_t index = HashMap_hash(entry->key->value) % capacity;

			entry->next = entries[index];
			entries[index] = entry;
			entry = next;
		}
	}

	// mem_free the old entries and set the new ones
	mem_free(map->entries);
	map->entries = entries;
	map->capacity = capacity;
}

void HashMap_set(HashMap *map, char *key, void *value) {
	if(!map) return;
	if(!key) return;

	// Resize the map if needed
	if(map->size >= map->capacity * HASHMAP_LOAD_FACTOR) {
		HashMap_resize(map, map->capacity * HASHMAP_RESIZE_FACTOR);
	}

	// Get the bucket index
	uint32_t index = HashMap_hash(key) % map->capacity;
	HashMapEntry *entry = map->entries[index];

	// If there is an entry with the same key, set it
	HashMapEntry *lastEntry = NULL; // Cache the last entry for later use
	while(entry) {
		if(String_equals(entry->key, key)) {
			entry->value = value;
			return;
		}

		lastEntry = entry;
		entry = entry->next;
	}

	// Create a new entry
	HashMapEntry *newEntry = mem_alloc(sizeof(HashMapEntry));
	if(!newEntry) return;

	// Create a new string for the key
	String *newKey = String_alloc(key);
	if(!newKey) return;

	// Setup the new entry
	newEntry->key = newKey;
	newEntry->value = value;
	newEntry->next = NULL;

	// Add the new entry to the bucket
	if(!lastEntry) map->entries[index] = newEntry;
	else lastEntry->next = newEntry;

	// Increase the size
	map->size++;
}

void* HashMap_get(HashMap *map, char *key) {
	if(!map) return NULL;
	if(!key) return NULL;

	// Get the bucket index
	uint32_t index = HashMap_hash(key) % map->capacity;
	HashMapEntry *entry = map->entries[index];

	// Find the entry with the same key
	while(entry) {
		if(String_equals(entry->key, key)) {
			return entry->value;
		}

		entry = entry->next;
	}

	return NULL;
}

bool HashMap_has(HashMap *map, char *key) {
	return HashMap_get(map, key) != NULL;
}

void HashMap_remove(HashMap *map, char *key) {
	if(!map) return;
	if(!key) return;

	// Get the bucket index
	uint32_t index = HashMap_hash(key) % map->capacity;
	HashMapEntry *entry = map->entries[index];

	// Find the entry with the same key
	HashMapEntry *lastEntry = NULL; // Cache the last entry for later use
	while(entry) {
		if(String_equals(entry->key, key)) {
			// Remove the entry from the bucket
			if(!lastEntry) map->entries[index] = entry->next;
			else lastEntry->next = entry->next;

			// mem_free the entry
			String_free(entry->key);
			mem_free(entry);

			// Decrease the size
			map->size--;

			return;
		}

		lastEntry = entry;
		entry = entry->next;
	}
}

void HashMap_clear(HashMap *map) {
	if(!map) return;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			HashMapEntry *next = entry->next;
			String_free(entry->key);
			mem_free(entry);
			entry = next;
		}
	}

	map->size = 0;
}

Array* HashMap_keys(HashMap *map) {
	if(!map) return NULL;

	Array *keys = Array_alloc(map->size);
	if(!keys) return NULL;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			Array_push(keys, entry->key);
			entry = entry->next;
		}
	}

	return keys;
}

Array* HashMap_values(HashMap *map) {
	if(!map) return NULL;

	Array *values = Array_alloc(map->size);
	if(!values) return NULL;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			Array_push(values, entry->value);
			entry = entry->next;
		}
	}

	return values;
}

void HashMap_forEach(HashMap *map, void (*callback)(String *key, void *value)) {
	if(!map) return;
	if(!callback) return;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			callback(entry->key, entry->value);
			entry = entry->next;
		}
	}
}

HashMap* HashMap_alloc() {
	HashMap *map = mem_alloc(sizeof(HashMap));
	if(!map) return NULL;

	HashMap_constructor(map);

	return map;
}

void HashMap_free(HashMap *map) {
	if(!map) return;

	HashMap_destructor(map);
	mem_free(map);
}

void HashMap_print(HashMap *map, unsigned int depth, int isProperty) {
	if(!map) {
		if(!isProperty) indent(depth);
		printf("HashMap { NULL }\n");
		return;
	}

	if(!isProperty) indent(depth);
	printf("HashMap {\n");

	indent(depth + 1);
	printf("entries: {\n");

	size_t index = 0;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			indent(depth + 2);
			printf("%zu: {\n", index++);

			indent(depth + 3);
			printf("key: ");
			String_print(entry->key, depth + 3, 1);

			indent(depth + 3);
			printf("value: ");
			if(entry->value) printf("<Object at %p>\n", entry->value);
			else printf("NULL\n");

			indent(depth + 2);
			if(index == map->size) printf("},\n");
			else printf("}\n");

			entry = entry->next;
		}
	}

	indent(depth + 1);
	printf("},\n");

	indent(depth + 1);
	printf("size: %zu,\n", map->size);

	indent(depth + 1);
	printf("capacity: %zu,\n", map->capacity);
}
