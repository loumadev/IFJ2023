#include "compiler/lexer/Token.h"
#include "allocator/MemoryAllocator.h"
#include "inspector.h"

void Token_constructor(Token *token, enum TokenType type, enum TokenKind kind, enum WhitespaceType whitespace, TextRange range, union TokenValue value) {
	if(!token) return;

	token->type = type;
	token->kind = kind;
	token->whitespace = whitespace;
	token->range = range;
	token->value = value;
}

void Token_destructor(Token *token) {
	if(!token) return;

	if(token->kind == TOKEN_STRING) {
		String_free(token->value.string);
		token->value.string = NULL;
	} else if(token->type == TOKEN_IDENTIFIER) {
		String_free(token->value.identifier);
		token->value.identifier = NULL;
	}

	token->type = TOKEN_INVALID;
	token->whitespace = WHITESPACE_NONE;
	TextRange_destructor(&token->range);
}

Token* Token_alloc(enum TokenType type, enum TokenKind kind, enum WhitespaceType whitespace, TextRange range, union TokenValue value) {
	Token *token = mem_alloc(sizeof(Token));
	if(!token) return NULL;

	Token_constructor(token, type, kind, whitespace, range, value);
	return token;
}

void Token_free(Token *token) {
	if(!token) return;

	Token_destructor(token);
	mem_free(token);
}

void Token_print(Token *token, unsigned int depth, int isProperty) {
	if(!token) {
		print_null_type("Token");
		return;
	}

	print_type_begin("Token");

	print_field("type", NUMBER "%d", token->type);
	print_field("kind", NUMBER "%d", token->kind);
	print_field("whitespace", NUMBER "%d", token->whitespace);

	print_field("value");
	if(token->type == TOKEN_EOF) println(POINTER "EOF");
	else if(token->type == TOKEN_IDENTIFIER) String_print(token->value.identifier, depth, 1);
	else if(token->kind == TOKEN_STRING) String_print(token->value.string, depth, 1);
	else if(token->kind == TOKEN_INTEGER) println(NUMBER "%ld", token->value.integer);
	else if(token->kind == TOKEN_FLOATING) println(NUMBER "%lf", token->value.floating);
	else println(GREY "Unknown");

	print_field("range");
	TextRange_print(&token->range, depth, 1);

	print_type_end();
}

char* Token_toString(Token *token) {
	if(!token) {
		return "null";
	}

	if(token->type == TOKEN_EOF) return "EOF";
	else if(token->type == TOKEN_IDENTIFIER) return token->value.identifier->value;
	else if(token->kind == TOKEN_STRING) return token->value.string->value;
	else if(token->kind == TOKEN_INTEGER) return String_fromLong(token->value.integer)->value;
	else if(token->kind == TOKEN_FLOATING) return String_fromDouble(token->value.floating)->value;

	return "unknown";
}
