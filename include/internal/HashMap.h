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

/**
 * Initializes a new HashMap instance.
 *
 * @param map The HashMap instance to initialize.
 */
void HashMap_constructor(HashMap *map);

/**
 * Deallocates the memory used by the given HashMap instance.
 *
 * @param map The HashMap instance to deallocate.
 */
void HashMap_destructor(HashMap *map);

/**
 * Associates the specified value with the specified key in this map.
 *
 * @param map The HashMap instance to add the key-value pair to.
 * @param key The key with which to associate the value.
 * @param value The value to associate with the key.
 */
void HashMap_set(HashMap *map, char *key, void *value);

/**
 * Returns the value to which the specified key is mapped, or NULL if this map contains no mapping for the key.
 *
 * @param map The HashMap instance to get the value from.
 * @param key The key whose associated value is to be returned.
 * @return The value to which the specified key is mapped, or NULL if this map contains no mapping for the key.
 */
void* HashMap_get(HashMap *map, char *key);

/**
 * Returns true if this map contains a mapping for the specified key.
 *
 * @param map The HashMap instance to check for the key.
 * @param key The key to check for.
 * @return True if this map contains a mapping for the specified key.
 */
bool HashMap_has(HashMap *map, char *key);

/**
 * Removes the mapping for the specified key from this map if present.
 *
 * @param map The HashMap instance to remove the key-value pair from.
 * @param key The key whose mapping is to be removed from the map.
 */
void HashMap_remove(HashMap *map, char *key);

/**
 * Removes all of the mappings from this map.
 *
 * @param map The HashMap instance to clear.
 */
void HashMap_clear(HashMap *map);

/**
 * Returns an array containing all of the keys in this map.
 *
 * @param map The HashMap instance to get the keys from.
 * @return An array containing all of the keys in this map.
 */
Array* HashMap_keys(HashMap *map);

/**
 * Returns an array containing all of the values in this map.
 *
 * @param map The HashMap instance to get the values from.
 * @return An array containing all of the values in this map.
 */
Array* HashMap_values(HashMap *map);

/**
 * Performs the given action for each entry in this map until all entries have been processed or the action throws an exception.
 *
 * @param map The HashMap instance to iterate over.
 * @param callback The action to be performed for each entry.
 */
void HashMap_forEach(HashMap *map, void (*callback)(String *key, void *value));

/**
 * Allocates memory for a new HashMap instance.
 *
 * @return A pointer to the newly allocated HashMap instance.
 */
HashMap* HashMap_alloc();

/**
 * Deallocates the memory used by the given HashMap instance.
 *
 * @param map The HashMap instance to deallocate.
 */
void HashMap_free(HashMap *map);

/**
 * Prints the contents of the HashMap in a compact format.
 *
 * @param map The HashMap instance to print.
 */
void HashMap_print_compact(HashMap *map);

/**
 * Prints the contents of the HashMap.
 *
 * @param map The HashMap instance to print.
 * @param depth The current depth of the printing. (default: 0) (used internally)
 * @param isProperty Whether the HashMap is a property of a larger object. (default: 0) (used internally)
 */
void HashMap_print(HashMap *map, unsigned int depth, int isProperty);

#endif
