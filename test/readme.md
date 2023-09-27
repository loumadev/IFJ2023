# Using test framework

**To use the test framework:**

1. Create a file named `<name>.test.c` somewhere in the `test/` directory (follow the `src/` directory structure if possible)
2. Import the framework using `#include "unit.h"`
3. Create a test suite by using `DESCRIBE(<unique_id>, "<description>") { ... }`, where `<unique_id>` is a unique identifier of the test suite (should be unique between files as well) and `<description>` is a human readable description of the test suite
4. Inside a test suite define a test case using `TEST("<some_description>", { <your_code_here> })` or `TEST_BEGIN("<some_description>") { <your_code_here> } TEST_END()` (the latter is debuggable, cuz debugger cannot step into a macro)
5. Inside a test case use macros defined in `unit.h`, like `EXPECT_TRUE(<expression>)` to test your code
6. When you are done with writing test, use Makefile target `make test` to run the tests
7. Progress and results of the tests will be printed to the stdout

// TODO: Maybe add a setup and teardown function to test suite

// TODO: Setup test target to run tests silently

**Example test file:**

`MyTests.test.c`
```c
#include "unit.h"

DESCRIBE(test_suite1, "Test suite 1") {
	TEST("Check equality of integers", {
		EXPECT_TRUE(1 == 1);
		EXPECT_FALSE(1 == 2);
	})

	TEST("Check inequality of strings", {
		EXPECT_NOT_EQUAL_STRING("Hello", "World");
	})
}

DESCRIBE(test_suite2, "Test suite 2") {
	// This test is debuggable
	TEST_BEGIN("Check inequality of characters") {
		// > can step here
		EXPECT_EQUAL_CHAR('A', 'B');
	} TEST_END()

	TEST("Check equality of floats", {
		EXPECT_EQUAL_FLOAT(3.14, 3.14);
	})
}
```

**Example output:**

```
MyTests.test.c
 • Test suite 1 (at test/MyTests.test.c:3)
    ✓  Test case 1: Check equality of integers (0ms)
    ✓  Test case 2: Check inequality of strings (0ms)
 • Test suite 2 (at test/MyTests.test.c:15)
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
