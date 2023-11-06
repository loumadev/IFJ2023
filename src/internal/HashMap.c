#include <stdio.h>

#include "internal/HashMap.h"
#include "allocator/MemoryAllocator.h"
#include "inspector.h"

void HashMap_constructor(HashMap *map) {
	if(!map) return;

	map->size = 0;
	map->capacity = HASHMAP_DEFAULT_CAPACITY;
	map->entries = mem_calloc(map->capacity, sizeof(HashMapEntry));
}

void HashMap_destructor(HashMap *map) {
	if(!map) return;

	for(size_t i = 0; i < map->size; i++) {
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;

		String_free(entry.key);
	}

	mem_free(map->entries);
	map->entries = NULL;
	map->capacity = 0;
	map->size = 0;
}

// Private
uint32_t HashMap_hash(char *key, size_t capacity) {
	uint32_t hash = 0;

	for(size_t i = 0; key[i] != '\0'; i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash % capacity;
}

// Private
void HashMap_resize(HashMap *map, size_t new_capacity) {
	if(!map) return;

	// Create a new array of entries
	HashMapEntry *new_entries = mem_calloc(new_capacity, sizeof(HashMapEntry));
	if(!new_entries) return;

	// Rehash and copy existing entries
	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry entry = map->entries[i];

		// Skip empty and deleted entries
		if(!entry.key) continue;
		if(entry.deleted) continue;

		// Rehash the key
		size_t index = HashMap_hash(entry.key->value, new_capacity);

		// Linear probing
		while(new_entries[index].key) {
			index = (index + 1) % new_capacity;
		}

		new_entries[index].key = entry.key;
		new_entries[index].value = entry.value;
	}

	// Free the old entries and set the new ones
	mem_free(map->entries);
	map->entries = new_entries;
	map->capacity = new_capacity;
}

void HashMap_set(HashMap *map, char *key, void *value) {
	if(!map) return;
	if(!key) return;

	// Resize the map if needed
	if(map->size >= map->capacity * HASHMAP_LOAD_FACTOR) {
		HashMap_resize(map, map->capacity * HASHMAP_RESIZE_FACTOR);
	}

	// Get the initial index
	size_t index = HashMap_hash(key, map->capacity);
	HashMapEntry entry = map->entries[index];

	// Find an empty or deleted slot
	while(entry.key && !entry.deleted) {
		// Update already existing entry
		if(String_equals(entry.key, key)) {
			entry.value = value;
			map->entries[index] = entry;

			return;
		}

		// Linear probing
		index = (index + 1) % map->capacity;
		entry = map->entries[index];
	}

	// Create a new entry
	entry.key = String_alloc(key);
	entry.value = value;
	entry.deleted = 0;

	map->entries[index] = entry;
	map->size++;
}

void* HashMap_get(HashMap *map, char *key) {
	if(!map) return NULL;
	if(!key) return NULL;

	// Get the initial index
	size_t index = HashMap_hash(key, map->capacity);
	HashMapEntry entry = map->entries[index];

	// Search for the key
	while(entry.key) {
		if(!entry.deleted && String_equals(entry.key, key)) {
			return entry.value;
		}

		index = (index + 1) % map->capacity; // Linear probing
		entry = map->entries[index];
	}

	return NULL;
}

void HashMap_remove(HashMap *map, char *key) {
	if(!map) return;
	if(!key) return;

	// Get the initial index
	size_t index = HashMap_hash(key, map->capacity);
	HashMapEntry entry = map->entries[index];

	// Search for the key
	while(entry.key) {
		if(!entry.deleted && String_equals(entry.key, key)) {
			// Mark the entry as deleted
			String_free(entry.key);

			entry.key = NULL;
			entry.value = NULL;
			entry.deleted = 1;

			map->entries[index] = entry;
			map->size--;

			return;
		}

		// Linear probing
		index = (index + 1) % map->capacity;
		entry = map->entries[index];
	}
}

bool HashMap_has(HashMap *map, char *key) {
	return HashMap_get(map, key) != NULL;
}

void HashMap_clear(HashMap *map) {
	if(!map) return;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;

		String_free(entry.key);

		entry.key = NULL;
		entry.value = NULL;
		entry.deleted = 0;

		map->entries[i] = entry;
	}

	map->size = 0;
}

Array* HashMap_keys(HashMap *map) {
	if(!map) return NULL;

	Array *keys = Array_alloc(map->size);
	if(!keys) return NULL;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;
		if(!entry.key) continue;

		Array_push(keys, entry.key);
	}

	return keys;
}

Array* HashMap_values(HashMap *map) {
	if(!map) return NULL;

	Array *values = Array_alloc(map->size);
	if(!values) return NULL;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;
		if(!entry.key) continue;

		Array_push(values, entry.value);
	}

	return values;
}

void HashMap_forEach(HashMap *map, void (*callback)(String *key, void *value, size_t index)) {
	if(!map) return;
	if(!callback) return;

	size_t index = 0;

	for(size_t i = 0; i < map->capacity; i++) {
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;

		callback(entry.key, entry.value, index++);
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
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;

		print_formatted_field("%s", entry.key->value);
		if(entry.value) print_pointer_field(entry.value);
		else print_null_field();
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
		HashMapEntry entry = map->entries[i];
		if(entry.deleted) continue;

		#ifdef HASHMAP_PRINT_COMPACT
		print_formatted_field("%s", entry.key->value);
		if(entry.value) print_pointer_field(entry.value);
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
	}

	print_obj_end();

	print_field("size", NUMBER "%zu", map->size);
	print_field("capacity", NUMBER "%zu", map->capacity);

	print_type_end();
}
