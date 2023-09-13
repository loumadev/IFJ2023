#include "utils.h"
#include "compiler/lexer/Token.h"
#include "allocator/MemoryAllocator.h"

void Token_constructor(Token *token, enum TokenType type, union TokenValue value, TextRange range) {
	if(!token) return;

	token->type = type;
	token->value = value;
	token->range = range;
}

void Token_destructor(Token *token) {
	if(!token) return;

	if(token->type == TOKEN_STRING) {
		String_free(token->value.string);
		token->value.string = NULL;
	} else if(token->type & TOKEN_IDENTIFIER) {
		mem_free(token->value.identifier);
		token->value.identifier = NULL;
	}

	token->type = TOKEN_INVALID;
	TextRange_destructor(&token->range);
}

Token* Token_alloc(enum TokenType type, union TokenValue value, TextRange range) {
	Token *token = mem_alloc(sizeof(Token));
	if(!token) return NULL;

	Token_constructor(token, type, value, range);
	return token;
}

void Token_free(Token *token) {
	if(!token) return;

	Token_destructor(token);
	mem_free(token);
}

void Token_print(Token *token, unsigned int depth, int isProperty) {
	if(!token) {
		if(!isProperty) indent(depth);
		printf("Token { NULL }\n");
		return;
	}

	if(!isProperty) indent(depth);
	printf("Token {\n");

	indent(depth + 1);
	printf("type: %d\n", token->type);

	indent(depth + 1);
	printf("value: ");
	if(token->type == TOKEN_STRING) String_print(token->value.string, depth + 1, 1);
	else if(token->type == TOKEN_NUMBER) printf("%f\n", token->value.number);
	else if(token->type & TOKEN_IDENTIFIER) print_string(token->value.identifier, NULL), putchar('\n');
	else printf("Unknown\n");

	indent(depth + 1);
	printf("range: ");
	TextRange_print(&token->range, depth + 1, 1);

	indent(depth);
	printf("}\n");
}
