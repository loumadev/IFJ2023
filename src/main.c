#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"


#include "allocator/MemoryAllocator.h"
#include "internal/HashMap.h"
#include "compiler/lexer/Lexer.h"
#include "inspector.h"
#include "assertf.h"

int main(int argc, const char *argv[]) {
	(void)argc;
	(void)argv;

	String *str = String_alloc("Hello World!");
	// String_print(str, 0, 0);

	int num = 123;
	char *str2 = "Test string";

	dumpvar(num, str2, str);

	Lexer lexer;
	Lexer_constructor(&lexer);

	assertf(num == 0, "num is not 0");

	// Result result = Lexer_tokenize(&lexer, "  TestIdentifier  Hello World 3.141592 \"My \\\'Test\\\" String\" ");
	// LexerResult result = Lexer_tokenize(&lexer, "let my_var = \"My \\\'Test\\\" String\";");

	// TODO: Add tests
	// LexerResult result = Lexer_tokenize(&lexer, "\"\"");
	// LexerResult result = Lexer_tokenize(&lexer, "\"t\"");
	// LexerResult result = Lexer_tokenize(&lexer, "\"test\"");
	// LexerResult result = Lexer_tokenize(&lexer, "\"some 'quoted' text\"");
	// LexerResult result = Lexer_tokenize(&lexer, "\"some \\\"quoted\\\" text\"");

	// LexerResult result = Lexer_tokenize(&lexer, "test");
	// LexerResult result = Lexer_tokenize(&lexer, "test ");
	// LexerResult result = Lexer_tokenize(&lexer, "t");
	// LexerResult result = Lexer_tokenize(&lexer, "t ");
	// LexerResult result = Lexer_tokenize(&lexer, "t3_st");
	// LexerResult result = Lexer_tokenize(&lexer, "_test");
	// LexerResult result = Lexer_tokenize(&lexer, "3t");   // OK?
	// LexerResult result = Lexer_tokenize(&lexer, "3test");   // OK?

	LexerResult result = Lexer_tokenize(&lexer, "7");
	// LexerResult result = Lexer_tokenize(&lexer, "7 ");
	// LexerResult result = Lexer_tokenize(&lexer, "7 a");
	// LexerResult result = Lexer_tokenize(&lexer, "256");
	// LexerResult result = Lexer_tokenize(&lexer, "3.14159265359");
	// LexerResult result = Lexer_tokenize(&lexer, "0.5");
	// LexerResult result = Lexer_tokenize(&lexer, ".5");   // Error
	// LexerResult result = Lexer_tokenize(&lexer, "0b");   // Error
	// LexerResult result = Lexer_tokenize(&lexer, "0bA");   // Error
	// LexerResult result = Lexer_tokenize(&lexer, "0b1");
	// LexerResult result = Lexer_tokenize(&lexer, "0b10");
	// LexerResult result = Lexer_tokenize(&lexer, "0o123");
	// LexerResult result = Lexer_tokenize(&lexer, "0x1aB");

	if(result.success) {
		printf("Success!\n");

		for(int i = 0; i < (int)lexer.tokens->size; i++) {
			Token *token = lexer.tokens->data[i];

			Token_print(token, 0, 0);

			// dumpvar(i, token->value.identifier);

			// if(token->type & TOKEN_IDENTIFIER) {
			// 	printf("Identifier: %s\n", token->value.identifier);
			// }
			// else if(token->type == TOKEN_STRING) {
			// 	printf("String: '%s'\n", token->value.string->value);
			// }
			// else if(token->type == TOKEN_NUMBER) {
			// 	printf("Number: %f\n", token->value.number);
			// }
			// else if(token->type == TOKEN_EOF) {
			// 	printf("EOF\n");
			// }
			// else {
			// 	printf("Token: %d\n", token->type);
			// }
		}
	} else {
		// TODO: Add some error handling here
		printf("Failure!\n");
		printf(RED "Error: %s\n" RST, result.message->value);
	}

	Lexer_destructor(&lexer);

	Allocator_cleanup();
	return 0;
}
