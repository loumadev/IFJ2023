#include "internal/Array.h"
#include "unit.h"
#include <stdio.h>

#define TEST_PRIORITY 100

DESCRIBE(get, "Array_get") {
	Array *arr = NULL;

	TEST("Get value from empty array", {
		arr = Array_alloc(10);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Get value from non-empty array", {
		int num1 = 10;
		int num2 = 20;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);

		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 20);
	})

	TEST("Get value from out-of-bounds index", {
		int num = 10;

		arr = Array_alloc(10);
		Array_push(arr, &num);

		EXPECT_EQUAL_PTR(Array_get(arr, 1), NULL);
	})

	TEST("Get value from negative index", {
		int num1 = 10;
		int num2 = 20;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);

		EXPECT_EQUAL_INT(*((int*)Array_get(arr, -1)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, -2)), 10);
	})
}

DESCRIBE(set, "Array_set") {
	Array *arr = NULL;
	TEST("Set value in empty array", {
		int num = 10;

		arr = Array_alloc(10);
		Array_set(arr, 0, &num);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);


		arr = Array_alloc(10);
		Array_set(arr, 2, &num);

		EXPECT_EQUAL_INT(arr->size, 3);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
		EXPECT_EQUAL_PTR(Array_get(arr, 1), NULL);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), 10);


		arr = Array_alloc(3);
		Array_set(arr, 10, &num);
		Array_set(arr, 4, &num);

		EXPECT_EQUAL_INT(arr->size, 11);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 4)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 10)), 10);


		for(int i = 0; i < 10; i++) {
			if(i == 4) continue;
			EXPECT_EQUAL_PTR(Array_get(arr, i), NULL);
		}
	})

	TEST("Set value in non-empty array", {
		int num1 = 1000;
		int num2 = 10;
		int num3 = 20;
		int num4 = 30;

		arr = Array_alloc(10);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);
		Array_set(arr, 1, &num1);

		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 1000);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), 30);
	})

	TEST("Set value at negative index", {
		int num1 = 100;
		int num2 = 10;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_set(arr, -1, &num2);

		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
	})
}


DESCRIBE(push, "Array_push") {
	Array *arr = NULL;

	TEST("Push one value", {
		int num = 10;

		arr = Array_alloc(1);
		Array_push(arr, &num);

		EXPECT_EQUAL_INT(arr->size, 1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
	})

	TEST("Push multiple values", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		EXPECT_EQUAL_INT(arr->size, 3);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), 30);
	})

	TEST("Push NULL value", {
		arr = Array_alloc(10);
		Array_push(arr, NULL);

		EXPECT_EQUAL_INT(arr->size, 1);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Push to empty array", {
		int num = 10;

		arr = Array_alloc(0);
		Array_push(arr, &num);

		EXPECT_EQUAL_INT(arr->size, 1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);

		Array_push(arr, &num);

		EXPECT_EQUAL_INT(arr->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 10);
	})

	TEST("Push to full array", {
		int num = 10;

		arr = Array_alloc(1);
		Array_push(arr, &num);

		EXPECT_EQUAL_INT(arr->size, 1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);

		Array_push(arr, &num);

		EXPECT_EQUAL_INT(arr->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 10);
	})

	TEST_BEGIN("Push values of different types") {
		int num = 10;
		float float_num = 10.213;
		String *str = String_alloc("Lorem impsum");

		arr = Array_alloc(10);
		Array_push(arr, &num);
		Array_push(arr, "dolor sit amet");
		Array_push(arr, &float_num);
		Array_push(arr, str);

		EXPECT_EQUAL_INT(arr->size, 4);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_STRING((char*)Array_get(arr, 1), "dolor sit amet");
		EXPECT_EQUAL_FLOAT(*(float*)Array_get(arr, 2), float_num);
		EXPECT_EQUAL_STRING(((String*)Array_get(arr, 3))->value, "Lorem impsum");
	} TEST_END();
}

DESCRIBE(pop, "Array_pop") {
	Array *arr = NULL;

	TEST("Pop from empty array", {
		arr = Array_alloc(10);
		EXPECT_EQUAL_PTR(Array_pop(arr), NULL);
	})

	TEST("Pop last value", {
		int num = 10;

		arr = Array_alloc(10);
		Array_push(arr, &num);

		int *out = (int*)Array_pop(arr);
		EXPECT_EQUAL_INT(*out, 10);
		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Pop multiple values", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		int *out1 = (int*)Array_pop(arr);
		int *out2 = (int*)Array_pop(arr);
		int *out3 = (int*)Array_pop(arr);

		EXPECT_EQUAL_INT(*out1, 30);
		EXPECT_EQUAL_INT(*out2, 20);
		EXPECT_EQUAL_INT(*out3, 10);
		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Pop values of different types", {
		int num = 10;
		float float_num = 10.213;
		String *str = String_alloc("Lorem impsum");

		arr = Array_alloc(10);
		Array_push(arr, &num);
		Array_push(arr, "dolor sit amet");
		Array_push(arr, &float_num);
		Array_push(arr, str);

		String *out1 = Array_pop(arr);
		float *out2 = Array_pop(arr);
		char *out3 = Array_pop(arr);
		int *out4 = Array_pop(arr);

		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_STRING(out1->value, "Lorem impsum");
		EXPECT_EQUAL_FLOAT(*out2, float_num);
		EXPECT_EQUAL_STRING(out3, "dolor sit amet");
		EXPECT_EQUAL_INT(*out4, num);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})
}

DESCRIBE(insert, "Array_insert") {
	Array *arr = NULL;

	TEST("Inserts a value into empty array", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_alloc(10);
		Array_insert(arr, 0, &num1);

		EXPECT_EQUAL_INT(arr->size, 1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), num1);

		Array_insert(arr, 0, &num2);

		EXPECT_EQUAL_INT(arr->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), num2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), num1);

		Array_insert(arr, 1, &num3);

		EXPECT_EQUAL_INT(arr->size, 3);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), num2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), num3);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), num1);
	})

	TEST("Inserts a value into non-empty array", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		Array_insert(arr, 1, &num4);

		EXPECT_EQUAL_INT(arr->size, 4);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), num1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), num4);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), num2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 3)), num3);
	})

	TEST("Inserts a value past the end", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		Array_insert(arr, 3, &num4);

		EXPECT_EQUAL_INT(arr->size, 4);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), num1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), num2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), num3);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 3)), num4);
	})

	TEST("Inserts a value at negative index", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		Array_insert(arr, -1, &num4);

		EXPECT_EQUAL_INT(arr->size, 4);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), num1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), num2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), num4);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 3)), num3);
	})
}

DESCRIBE(remove, "Array_remove") {
	Array *arr = NULL;

	TEST("Remove value from empty array", {
		arr = Array_alloc(10);
		EXPECT_EQUAL_PTR(Array_remove(arr, 0), NULL);
	})

	TEST("Remove value from non-empty array", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		Array_remove(arr, 1);
		EXPECT_EQUAL_INT(arr->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 30);
	})

	TEST("Remove last value from array", {
		int num1 = 10;
		int num2 = 20;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);

		Array_remove(arr, 1);
		Array_remove(arr, 0);
		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Remove value from beginning - shifting", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_alloc(2);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		Array_remove(arr, 0); // arr[0] == 10
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 20);

		Array_remove(arr, 0); // arr[0] == 20
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 30);

		Array_remove(arr, 0); // arr[0] == 30
		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Remove value from array with one element", {
		int num = 10;

		arr = Array_alloc(10);
		Array_push(arr, &num);

		Array_remove(arr, 0);
		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})
}

DESCRIBE(clear, "Array_clear") {
	Array *arr = NULL;

	TEST("Clear empty array", {
		arr = Array_alloc(10);
		Array_clear(arr);

		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Clear non-empty array", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);

		Array_clear(arr);
		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})
}

DESCRIBE(arr_slice, "Array_slice") {
	Array *arr = NULL;

	TEST("Slices empty array", {
		arr = Array_alloc(10);

		Array *slice = Array_slice(arr, 1, 3);
		EXPECT_EQUAL_INT(slice->size, 0);
		EXPECT_EQUAL_PTR(Array_get(slice, 0), NULL);
	})

	TEST("Slices non-empty array", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);

		Array *slice = Array_slice(arr, 1, 3);
		EXPECT_EQUAL_INT(slice->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 0)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 1)), 30);
	})

	TEST("Slices array with negative indices", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);

		Array *slice = Array_slice(arr, -3, -1);
		EXPECT_EQUAL_INT(slice->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 0)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 1)), 30);
	})

	TEST("Slices array with out-of-bounds indices", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);

		Array *slice = Array_slice(arr, -3, 10);
		EXPECT_EQUAL_INT(slice->size, 3);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 0)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 1)), 30);
		EXPECT_EQUAL_INT(*((int*)Array_get(slice, 2)), 40);
	})
}

DESCRIBE(arr_splice, "Array_splice") {
	Array *arr = NULL;

	TEST("Splices empty array", {
		arr = Array_alloc(10);

		Array *splice = Array_splice(arr, 1, 3);
		EXPECT_EQUAL_INT(splice->size, 0);
		EXPECT_EQUAL_PTR(Array_get(splice, 0), NULL);
	})

	TEST("Splices non-empty array", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);

		Array *splice = Array_splice(arr, 1, 3);
		EXPECT_EQUAL_INT(splice->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 0)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 1)), 30);
	})

	TEST("Splices array with negative indices", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);

		Array *splice = Array_splice(arr, -3, -1);
		EXPECT_EQUAL_INT(splice->size, 2);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 0)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 1)), 30);
	})

	TEST("Splices array with out-of-bounds indices", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;
		int num4 = 40;

		arr = Array_alloc(10);
		Array_push(arr, &num1);
		Array_push(arr, &num2);
		Array_push(arr, &num3);
		Array_push(arr, &num4);

		Array *splice = Array_splice(arr, -3, 10);
		EXPECT_EQUAL_INT(splice->size, 3);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 0)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 1)), 30);
		EXPECT_EQUAL_INT(*((int*)Array_get(splice, 2)), 40);
	})
}

DESCRIBE(Array_fromArgs, "Array_fromArgs") {
	Array *arr = NULL;

	TEST("Create array from arguments", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_fromArgs(3, &num1, &num2, &num3);

		EXPECT_EQUAL_INT(arr->size, 3);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 1)), 20);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), 30);
	});

	TEST("Create array from arguments with NULL values", {
		int num1 = 10;
		int num2 = 20;
		int num3 = 30;

		arr = Array_fromArgs(5, &num1, NULL, &num2, NULL, &num3);

		EXPECT_EQUAL_INT(arr->size, 5);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
		EXPECT_EQUAL_PTR(Array_get(arr, 1), NULL);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 2)), 20);
		EXPECT_EQUAL_PTR(Array_get(arr, 3), NULL);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 4)), 30);
	})

	TEST("Create array from arguments with none values", {
		arr = Array_fromArgs(0);

		EXPECT_EQUAL_INT(arr->size, 0);
		EXPECT_EQUAL_PTR(Array_get(arr, 0), NULL);
	})

	TEST("Create array from arguments with single value", {
		int num = 10;

		arr = Array_fromArgs(1, &num);

		EXPECT_EQUAL_INT(arr->size, 1);
		EXPECT_EQUAL_INT(*((int*)Array_get(arr, 0)), 10);
	})
}
