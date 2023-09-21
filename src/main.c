#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"


#include "allocator/MemoryAllocator.h"
#include "internal/HashMap.h"
#include "compiler/lexer/Lexer.h"
#include "inspector.h"

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

	// Result result = Lexer_tokenize(&lexer, "  TestIdentifier  Hello World 3.141592 \"My \\\'Test\\\" String\" ");
	LexerResult result = Lexer_tokenize(&lexer, "let my_var = \"My \\\'Test\\\" String\";");
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
