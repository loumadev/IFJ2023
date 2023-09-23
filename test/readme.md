# Using test framework

**To use the test framework:**

1. Create a file named `<name>.test.c` somewhere in the `test/` directory (follow the `src/` directory structure if possible)
2. Import the framework using `#include "unit.h"`
3. Create a test suite by defining a function with the signature `void <name>(void)`, where `<name>` does NOT start with `_` (functions prefixed with `_` are considered private and are not tested!)
4. Inside a test suite define a test case using `TEST("<some_description>", { <your_code_here> })`
5. Inside a test case use macros defined in `unit.h`, like `EXPECT_TRUE(<expression>)` to test your code
6. When you are done with writing test, use Makefile target `make test` to run the tests
7. Progress and results of the tests will be printed to the stdout

// TODO: Maybe add a setup and teardown function to test suite

// TODO: Setup test target to run tests silently

**Example test file:**

`MyTests.test.c`
```c
#include "unit.h"

void test_suite1(void) {
	TEST("Check equality of integers", {
		EXPECT_TRUE(1 == 1);
		EXPECT_FALSE(1 == 2);
	})

	TEST("Check inequality of strings", {
		EXPECT_NOT_EQUAL_STRING("Hello", "World");
	})
}

void test_suite2(void) {
	TEST("Check inequality of characters", {
		EXPECT_EQUAL_CHAR('A', 'B');
	})

	TEST("Check equality of floats", {
		EXPECT_EQUAL_FLOAT(3.14, 3.14);
	})
}
```

**Example output:**

```
MyTests.test.c
 • test_suite1 (at test/MyTests.test.c:3)
    ✓  Test case 1: Check equality of integers (0ms)
    ✓  Test case 2: Check inequality of strings (0ms)
 • test_suite2 (at test/MyTests.test.c:14)
    ✕  Check inequality of characters   
         'A' == 'B'   Expected 'A' == 'B', got 'A' != 'B' (at test/MyTests.test.c:16)
    ✓  Check equality of floats (0ms)

Files:       1 failed, 1 total
Test Suites: 1 failed, 2 total
Tests:       1 failed, 4 total
Time:        0.001s
```

# List of available macros

Defined in `unit.h`:

```c
EXPECT_TRUE(condition)
EXPECT_FALSE(condition)

EXPECT_EQUAL_STRING(a, b)
EXPECT_EQUAL_CHAR(a, b)
EXPECT_EQUAL_INT(a, b)
EXPECT_EQUAL_FLOAT(a, b)

EXPECT_NOT_EQUAL_STRING(a, b)
EXPECT_NOT_EQUAL_CHAR(a, b)
EXPECT_NOT_EQUAL_INT(a, b)
EXPECT_NOT_EQUAL_FLOAT(a, b)

EXPECT_GREATER_CHAR(a, b)
EXPECT_GREATER_INT(a, b)
EXPECT_GREATER_FLOAT(a, b)

EXPECT_GREATER_OR_EQUAL_CHAR(a, b)
EXPECT_GREATER_OR_EQUAL_INT(a, b)
EXPECT_GREATER_OR_EQUAL_FLOAT(a, b)

EXPECT_LESS_CHAR(a, b)
EXPECT_LESS_INT(a, b)
EXPECT_LESS_FLOAT(a, b)

EXPECT_LESS_OR_EQUAL_CHAR(a, b)
EXPECT_LESS_OR_EQUAL_INT(a, b)
EXPECT_LESS_OR_EQUAL_FLOAT(a, b)
```
