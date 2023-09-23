#include "internal/String.h"
#include "unit.h"
#include <stdio.h>

void replaceAll() {
	String *str;

	TEST("Simple replacement", {
		str = String_alloc("Hello, World!");
		String_replaceAll(str, "Hello", "Goodbye");
		EXPECT_TRUE(String_equals(str, "Goodbye, World!"));

		str = String_alloc("SomeTestString");
		String_replaceAll(str, "Test", "Replacement");
		EXPECT_TRUE(String_equals(str, "SomeReplacementString"));
	})

	TEST("Single character edge replacement", {
		str = String_alloc("ABC");
		String_replaceAll(str, "A", "X");
		EXPECT_TRUE(String_equals(str, "XBC"));

		str = String_alloc("ABC");
		String_replaceAll(str, "B", "X");
		EXPECT_TRUE(String_equals(str, "AXC"));

		str = String_alloc("ABC");
		String_replaceAll(str, "C", "X");
		EXPECT_TRUE(String_equals(str, "ABX"));
	})

	TEST("Multiple character edge replacement", {
		str = String_alloc("AABBCC");
		String_replaceAll(str, "AA", "XX");
		EXPECT_TRUE(String_equals(str, "XXBBCC"));

		str = String_alloc("AABBCC");
		String_replaceAll(str, "BB", "XX");
		EXPECT_TRUE(String_equals(str, "AAXXCC"));

		str = String_alloc("AABBCC");
		String_replaceAll(str, "CC", "XX");
		EXPECT_TRUE(String_equals(str, "AABBXX"));
	})

	TEST("Empty replacement", {
		str = String_alloc("ABC");
		String_replaceAll(str, "A", "");
		EXPECT_TRUE(String_equals(str, "BC"));

		str = String_alloc("ABC");
		String_replaceAll(str, "B", "");
		EXPECT_TRUE(String_equals(str, "AC"));

		str = String_alloc("ABC");
		String_replaceAll(str, "C", "");
		EXPECT_TRUE(String_equals(str, "AB"));
	})

	TEST("Multiple occurrences replacement", {
		str = String_alloc("TestSomeTestingString");
		String_replaceAll(str, "Test", "Replace");
		EXPECT_TRUE(String_equals(str, "ReplaceSomeReplaceingString"));
	})

	TEST("Multiple occurrences empty replacement", {
		str = String_alloc("TestSomeTestString");
		String_replaceAll(str, "Test", "");
		EXPECT_TRUE(String_equals(str, "SomeString"));
	})

	TEST("No match replacement", {
		str = String_alloc("TestSomeTestString");
		String_replaceAll(str, "None", "");
		EXPECT_TRUE(String_equals(str, "TestSomeTestString"));

		str = String_alloc("TestSomeTestString");
		String_replaceAll(str, "None", "Something");
		EXPECT_TRUE(String_equals(str, "TestSomeTestString"));
	})
}
