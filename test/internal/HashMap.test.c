#include "internal/HashMap.h"
#include "unit.h"
#include <stdio.h>

#define TEST_PRIORITY 100

DESCRIBE(map_alloc, "HashMap_alloc/HashMap_resize") {
	HashMap *map = NULL;

	TEST("Safe memory after allocation", {
		map = HashMap_alloc();

		for(size_t i = 0; i < map->capacity; i++) {
			HashMapEntry entry = map->entries[i];
			EXPECT_NULL(entry.key);
			EXPECT_NULL(entry.value);
			EXPECT_EQUAL_INT(entry.deleted, 0);
		}
	})

	TEST("Safe memory after reallocation", {
		map = HashMap_alloc();

		for(size_t i = 0; i < map->capacity; i++) {
			HashMapEntry entry = map->entries[i];
			EXPECT_NULL(entry.key);
			EXPECT_NULL(entry.value);
			EXPECT_EQUAL_INT(entry.deleted, 0);
		}

		HashMap_resize(map, 100);

		for(size_t i = 0; i < map->capacity; i++) {
			HashMapEntry entry = map->entries[i];
			EXPECT_NULL(entry.key);
			EXPECT_NULL(entry.value);
			EXPECT_EQUAL_INT(entry.deleted, 0);
		}
	})
}

DESCRIBE(map_set_get, "HashMap_set/HashMap_get") {
	HashMap *map = NULL;
	int *value = NULL;

	TEST("Get value from an empty map", {
		map = HashMap_alloc();

		EXPECT_NULL(HashMap_get(map, ""));
		EXPECT_NULL(HashMap_get(map, "key1"));
		EXPECT_NULL(HashMap_get(map, "key123456789"));
	})

	TEST_BEGIN("Set and get a value") {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 20;

		HashMap_set(map, "key1", &num1);

		value = HashMap_get(map, "key1");
		EXPECT_NOT_NULL(value);
		EXPECT_EQUAL_INT(*value, num1);

		HashMap_set(map, "key2", &num2);
		HashMap_set(map, "key3", &num3);

		value = HashMap_get(map, "key2");
		EXPECT_NOT_NULL(value);
		EXPECT_EQUAL_INT(*value, num2);

		value = HashMap_get(map, "key3");
		EXPECT_NOT_NULL(value);
		EXPECT_EQUAL_INT(*value, num3);

		value = HashMap_get(map, "key1");
		EXPECT_NOT_NULL(value);
		EXPECT_EQUAL_INT(*value, num1);

		EXPECT_NULL(HashMap_get(map, "key4"));
		EXPECT_NULL(HashMap_get(map, "key5"));
	} TEST_END();

	TEST_BEGIN("Set and get a value with the same key") {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key1", &num2);
		HashMap_set(map, "key1", &num3);

		value = HashMap_get(map, "key1");
		EXPECT_NOT_NULL(value);
		EXPECT_EQUAL_INT(*value, num3);
	} TEST_END();
}

DESCRIBE(map_has, "HashMap_has") {
	HashMap *map = NULL;

	TEST("Check if a map has a key", {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key2", &num2);
		HashMap_set(map, "key3", &num3);

		EXPECT_TRUE(HashMap_has(map, "key1"));
		EXPECT_TRUE(HashMap_has(map, "key2"));
		EXPECT_TRUE(HashMap_has(map, "key3"));
		EXPECT_FALSE(HashMap_has(map, "key4"));
		EXPECT_FALSE(HashMap_has(map, "key5"));
	})

	TEST("Check if a map has a key with the same key", {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key1", &num2);
		HashMap_set(map, "key1", &num3);

		EXPECT_TRUE(HashMap_has(map, "key1"));
	})

	TEST("Check if a map has a key with an empty key", {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "", &num1);
		HashMap_set(map, "", &num2);
		HashMap_set(map, "", &num3);

		EXPECT_TRUE(HashMap_has(map, ""));
	})

	TEST("Check if an empty map has a key", {
		map = HashMap_alloc();

		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_FALSE(HashMap_has(map, "key2"));
	})
}

DESCRIBE(map_remove, "HashMap_remove") {
	HashMap *map = NULL;

	TEST("Remove a key from a map", {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key2", &num2);
		HashMap_set(map, "key3", &num3);

		EXPECT_TRUE(HashMap_has(map, "key1"));
		EXPECT_TRUE(HashMap_has(map, "key2"));
		EXPECT_TRUE(HashMap_has(map, "key3"));

		HashMap_remove(map, "key1");
		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_TRUE(HashMap_has(map, "key2"));
		EXPECT_TRUE(HashMap_has(map, "key3"));

		HashMap_remove(map, "key2");
		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_FALSE(HashMap_has(map, "key2"));
		EXPECT_TRUE(HashMap_has(map, "key3"));

		HashMap_remove(map, "key3");
		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_FALSE(HashMap_has(map, "key2"));
		EXPECT_FALSE(HashMap_has(map, "key3"));
	})

	TEST("Remove a key from a map with the same key", {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key1", &num2);
		HashMap_set(map, "key1", &num3);

		EXPECT_TRUE(HashMap_has(map, "key1"));

		HashMap_remove(map, "key1");
		EXPECT_FALSE(HashMap_has(map, "key1"));
	})

	TEST("Remove a key from an empty map", {
		map = HashMap_alloc();

		EXPECT_FALSE(HashMap_has(map, "key1"));

		HashMap_remove(map, "key1");
		EXPECT_FALSE(HashMap_has(map, "key1"));
	})
}

DESCRIBE(map_clear, "HashMap_clear") {
	HashMap *map = NULL;

	TEST_BEGIN("Clear a map") {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key2", &num2);
		HashMap_set(map, "key3", &num3);

		EXPECT_TRUE(HashMap_has(map, "key1"));
		EXPECT_TRUE(HashMap_has(map, "key2"));
		EXPECT_TRUE(HashMap_has(map, "key3"));

		HashMap_clear(map);

		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_FALSE(HashMap_has(map, "key2"));
		EXPECT_FALSE(HashMap_has(map, "key3"));
	} TEST_END();

	TEST("Clear an empty map", {
		map = HashMap_alloc();

		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_FALSE(HashMap_has(map, "key2"));
		EXPECT_FALSE(HashMap_has(map, "key3"));

		HashMap_clear(map);

		EXPECT_FALSE(HashMap_has(map, "key1"));
		EXPECT_FALSE(HashMap_has(map, "key2"));
		EXPECT_FALSE(HashMap_has(map, "key3"));
	})
}

DESCRIBE(map_keys, "HashMap_keys") {
	HashMap *map = NULL;

	TEST_BEGIN("Get keys from a map") {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key2", &num2);
		HashMap_set(map, "key3", &num3);

		Array *keys = HashMap_keys(map);
		EXPECT_EQUAL_INT(keys->size, 3);
		EXPECT_TRUE(String_equals((String*)Array_get(keys, 0), "key1"));
		EXPECT_TRUE(String_equals((String*)Array_get(keys, 1), "key2"));
		EXPECT_TRUE(String_equals((String*)Array_get(keys, 2), "key3"));
	} TEST_END();

	TEST("Get keys from an empty map", {
		map = HashMap_alloc();

		Array *keys = HashMap_keys(map);
		EXPECT_EQUAL_INT(keys->size, 0);
	})
}

DESCRIBE(map_values, "HashMap_values") {
	HashMap *map = NULL;

	TEST("Get values from a map", {
		map = HashMap_alloc();

		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		HashMap_set(map, "key1", &num1);
		HashMap_set(map, "key2", &num2);
		HashMap_set(map, "key3", &num3);

		Array *values = HashMap_values(map);
		EXPECT_EQUAL_INT(values->size, 3);
		EXPECT_EQUAL_INT(*((int*)Array_get(values, 0)), num1);
		EXPECT_EQUAL_INT(*((int*)Array_get(values, 1)), num2);
		EXPECT_EQUAL_INT(*((int*)Array_get(values, 2)), num3);
	})

	TEST("Get values from an empty map", {
		map = HashMap_alloc();

		Array *values = HashMap_values(map);
		EXPECT_EQUAL_INT(values->size, 0);
	})
}
