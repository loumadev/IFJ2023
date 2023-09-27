#include "internal/String.h"
#include "unit.h"
#include <stdio.h>

DESCRIBE(equals, "String_equals") {
	String *str = NULL;

	TEST("Simple equals", {
		str = String_alloc("Hello, World!");

		EXPECT_TRUE(String_equals(str, "Hello, World!"));
		EXPECT_FALSE(String_equals(str, "Hello!"));
		EXPECT_FALSE(String_equals(str, ""));
	})

	TEST("Partial equals", {
		str = String_alloc("ABC");

		EXPECT_TRUE(String_equals(str, "ABC"));
		EXPECT_FALSE(String_equals(str, "AB"));
		EXPECT_FALSE(String_equals(str, "A"));
		EXPECT_FALSE(String_equals(str, "B"));
	})

	TEST("Single character equals", {
		str = String_alloc("A");

		EXPECT_TRUE(String_equals(str, "A"));
		EXPECT_FALSE(String_equals(str, "B"));
		EXPECT_FALSE(String_equals(str, ""));
	})

	TEST("Empty equals", {
		str = String_alloc("");

		EXPECT_TRUE(String_equals(str, ""));
		EXPECT_FALSE(String_equals(str, "A"));
		EXPECT_FALSE(String_equals(str, "ABC"));
	})
}

DESCRIBE(replaceAll, "String_replaceAll") {
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

DESCRIBE(startsWith, "String_startsWith") {
	String *str = NULL;

	TEST("Simple startsWith", {
		str = String_alloc("Hello, World!");

		EXPECT_TRUE(String_startsWith(str, "Hello"));
		EXPECT_FALSE(String_startsWith(str, "World"));
		EXPECT_TRUE(String_startsWith(str, "Hello, World!"));
		EXPECT_FALSE(String_startsWith(str, "Hello, World! "));
	})

	TEST("Single character startsWith", {
		str = String_alloc("A");

		EXPECT_TRUE(String_startsWith(str, "A"));
		EXPECT_FALSE(String_startsWith(str, "B"));
		EXPECT_TRUE(String_startsWith(str, ""));
	})

	TEST("Empty startsWith", {
		str = String_alloc("");

		EXPECT_TRUE(String_startsWith(str, ""));
		EXPECT_FALSE(String_startsWith(str, "A"));
		EXPECT_FALSE(String_startsWith(str, "ABC"));
	})
}

DESCRIBE(endsWith, "String_endsWith") {
	String *str = NULL;

	TEST("Simple endsWith", {
		str = String_alloc("Hello, World!");

		EXPECT_TRUE(String_endsWith(str, "World!"));
		EXPECT_FALSE(String_endsWith(str, "Hello"));
		EXPECT_TRUE(String_endsWith(str, "Hello, World!"));
		EXPECT_FALSE(String_endsWith(str, " Hello, World!"));
	})

	TEST("Single character endsWith", {
		str = String_alloc("A");

		EXPECT_TRUE(String_endsWith(str, "A"));
		EXPECT_FALSE(String_endsWith(str, "B"));
		EXPECT_TRUE(String_endsWith(str, ""));
	})

	TEST("Empty endsWith", {
		str = String_alloc("");

		EXPECT_TRUE(String_endsWith(str, ""));
		EXPECT_FALSE(String_endsWith(str, "A"));
		EXPECT_FALSE(String_endsWith(str, "ABC"));
	})
}


DESCRIBE(indexOf, "String_indexOf") {
	String *str = NULL;

	TEST("Simple indexOf", {
		str = String_alloc("Hello, World!");

		EXPECT_TRUE(String_indexOf(str, "Hello") == 0);
		EXPECT_TRUE(String_indexOf(str, "World") == 7);
		EXPECT_TRUE(String_indexOf(str, "Hello, World!") == 0);
		EXPECT_TRUE(String_indexOf(str, "Hello, World! ") == -1);
		EXPECT_TRUE(String_indexOf(str, "Empty") == -1);
	})

	TEST("Single character indexOf", {
		str = String_alloc("A");

		EXPECT_TRUE(String_indexOf(str, "A") == 0);
		EXPECT_TRUE(String_indexOf(str, "B") == -1);
		EXPECT_TRUE(String_indexOf(str, "") == 0);
	})

	TEST("Empty indexOf", {
		str = String_alloc("");

		EXPECT_TRUE(String_indexOf(str, "") == 0);
		EXPECT_TRUE(String_indexOf(str, "A") == -1);
		EXPECT_TRUE(String_indexOf(str, "ABC") == -1);
	})
}

DESCRIBE(splice, "String_splice") {
	String *str = NULL;
	TEST("Simple splice", {
		str = String_alloc("Hello, World!");
		String_splice(str, 0, 5, "Goodbye");
		EXPECT_TRUE(String_equals(str, "Goodbye, World!"));
	})

	TEST("Splice one", {
		str = String_alloc("Hello, World!");
		String_splice(str, 0, 1, "G");
		EXPECT_TRUE(String_equals(str, "Gello, World!"));

		str = String_alloc("Hello, World!");
		String_splice(str, 3, 5, "Goodbye");
		EXPECT_TRUE(String_equals(str, "HelGoodbye, World!"));
	})

	TEST("Splice prepend", {
		str = String_alloc("Hello, World!");
		String_splice(str, 0, 0, "G");
		EXPECT_TRUE(String_equals(str, "GHello, World!"));
	})

	TEST("Splice append", {
		str = String_alloc("Hello, World!");
		String_splice(str, 13, 13, "G");
		EXPECT_TRUE(String_equals(str, "Hello, World!G"));
	})

	TEST("Splice insert", {
		str = String_alloc("Hello, World!");
		String_splice(str, 2, 2, "G");
		EXPECT_TRUE(String_equals(str, "HeGllo, World!"));
	})

	TEST("Splice out of bounds", {
		str = String_alloc("Hello, World!");
		String_splice(str, 20, 14, "G");
		EXPECT_TRUE(String_equals(str, "Hello, World!G"));

		str = String_alloc("Hello, World!");
		String_splice(str, 20, 1, "G");
		EXPECT_TRUE(String_equals(str, "Hello, World!G"));

		str = String_alloc("Hello, World!");
		String_splice(str, 2, 10000, "G");
		EXPECT_TRUE(String_equals(str, "HeG"));

		str = String_alloc("Hello, World!");
		String_splice(str, 13, 9, "Goodbye");
		EXPECT_TRUE(String_equals(str, "Hello, World!Goodbye"));
	})

}

DESCRIBE(slice, "String_slice") {
	String *str = NULL;
	String *out = NULL;

	TEST("Simple slice", {
		str = String_alloc("Hello, World!");
		out = String_slice(str, 0, 5);
		EXPECT_TRUE(String_equals(out, "Hello"));

		str = String_alloc("Hello, World!");
		out = String_slice(str, 1, 2);
		EXPECT_TRUE(String_equals(out, "e"));

		str = String_alloc("Hello, World!");
		out = String_slice(str, 1, 1);
		EXPECT_TRUE(String_equals(out, ""));

		str = String_alloc("Hello, World!");
		out = String_slice(str, 0, 13);
		EXPECT_TRUE(String_equals(out, "Hello, World!"));

		str = String_alloc("Hello, World!");
		out = String_slice(str, 0, 12);
		EXPECT_TRUE(String_equals(out, "Hello, World"));
	})

	TEST("Slice out of bounds", {
		str = String_alloc("Hello, World!");
		out = String_slice(str, -1, 1);
		EXPECT_TRUE(out == NULL);

		str = String_alloc("Hello, World!");
		out = String_slice(str, 14, 1);
		EXPECT_TRUE(out == NULL);

		str = String_alloc("Hello, World!");
		out = String_slice(str, 14, 15);
		EXPECT_TRUE(out == NULL);

		str = String_alloc("Hello, World!");
		out = String_slice(str, 13, 1000);
		EXPECT_TRUE(out == NULL);

		str = String_alloc("Hello, World!");
		out = String_slice(str, 12, 1000);
		EXPECT_TRUE(String_equals(out, "!"));
	})
}
