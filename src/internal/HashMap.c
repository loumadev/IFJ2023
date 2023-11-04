#include <stdio.h>

#include "internal/HashMap.h"
#include "allocator/MemoryAllocator.h"
#include "inspector.h"

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

		map->entries[i] = NULL;
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

void HashMap_forEach(HashMap *map, void (*callback)(String *key, void *value, size_t index)) {
	if(!map) return;
	if(!callback) return;

	size_t index = 0;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			callback(entry->key, entry->value, index++);
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

void HashMap_print_compact(HashMap *map) {
	if(!map) return (void)print_null_nl();

	// Need to set these to avoid useless arguments
	int isProperty = 0;
	unsigned int depth = 0;

	print_type_begin("HashMap");

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			print_formatted_field("%s", entry->key->value);
			if(entry->value) print_pointer_field(entry->value);
			else print_null_field();

			entry = entry->next;
		}
	}

	print_type_end();
}


void HashMap_print(HashMap *map, unsigned int depth, int isProperty) {
	if(!map) {
		print_null_type("HashMap");
		return;
	}

	print_type_begin("HashMap");

	print_field("entries");
	print_obj_begin(1);

	#define HASHMAP_PRINT_COMPACT

	size_t index = 0;
	(void)index;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry *entry = map->entries[i];
		if(!entry) continue;

		while(entry) {
			#ifdef HASHMAP_PRINT_COMPACT
			print_formatted_field("%s", entry->key->value);
			if(entry->value) print_pointer_field(entry->value);
			else print_null_field();
			#else
			print_formatted_field("%zu", index++);
			print_obj_begin(1);

			print_field("key");
			String_print(entry->key, depth, 1);

			print_field("value");
			if(entry->value) print_pointer_field(entry->value);
			else print_null_field();

			print_obj_end();
			#endif

			entry = entry->next;
		}
	}

	print_obj_end();

	print_field("size", NUMBER "%zu", map->size);
	print_field("capacity", NUMBER "%zu", map->capacity);

	print_type_end();
}
