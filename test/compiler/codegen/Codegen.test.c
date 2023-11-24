#include <stdio.h>

#include "unit.h"

#include "compiler/codegen/Codegen.h"
#include "internal/String.h"

#define TEST_PRIORITY 70

DESCRIBE(string_escape, "Escape special"){
    String * string;
    

	TEST_BEGIN("Simple variable declaration") {
        string = String_alloc("Hello World!");
		Codegen_escapeString(string);
		
        EXPECT_EQUAL_STRING(string->value, "Hello\\032World!");
	} TEST_END();

    TEST_BEGIN("Simple string from example") {
        string = String_alloc("retezec s lomitkem \\\nnovym#radkem");
		Codegen_escapeString(string);
		
        EXPECT_EQUAL_STRING(string->value, "retezec\032s\032lomitkem\032\092\032a\010novym\035radkem");
	} TEST_END();
    
}