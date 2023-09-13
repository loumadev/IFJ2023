#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

#include "allocator/MemoryAllocator.h"
#include "compiler/lexer/Lexer.h"

int main(int argc, const char *argv[]) {
	(void)argc;
	(void)argv;

	// String *str = String_alloc("Hello World!");
	// String_print(str, 0, 0);

	Lexer tokenizer;
	Lexer_constructor(&tokenizer);

	Result result = Lexer_tokenize(&tokenizer, "  TestIdentifier  Hello World 3.141592 \"My \\\'Test\\\" String\" ");
	if(result.success) {
		printf("Success!\n");

		for(int i = 0; i < (int)tokenizer.tokens->size; i++) {
			Token *token = tokenizer.tokens->data[i];

			Token_print(token, 0, 0);

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
		printf("Failure!\n");
	}

	Lexer_destructor(&tokenizer);

	Allocator_cleanup();
	return 0;
}
