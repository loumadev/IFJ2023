#include "internal/String.h"
#include "internal/TextRange.h"

enum TokenType {
	// Single-character tokens.
	TOKEN_PUNCTUATION = 0x00010000,
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
	TOKEN_COMMA, TOKEN_DOT, TOKEN_COLON, TOKEN_SEMICOLON,
	TOKEN_MINUS, TOKEN_PLUS, TOKEN_SLASH, TOKEN_ASTERISK,
	TOKEN_EXCLAMATION, TOKEN_QUESTION, TOKEN_PERCENT,
	TOKEN_CARET, TOKEN_TILDE, TOKEN_AMPERSAND, TOKEN_PIPE,

	// One or two character tokens.
	TOKEN_BANG, TOKEN_BANG_EQUAL,
	TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
	TOKEN_GREATER, TOKEN_GREATER_EQUAL,
	TOKEN_LESS, TOKEN_LESS_EQUAL,

	// Literals.
	TOKEN_LITERAL = 0x00020000,
	TOKEN_STRING, TOKEN_NUMBER,

	TOKEN_IDENTIFIER = 0x00040000, // Can be just a flag or a token type
	TOKEN_KEYWORD = 0x00080000,

	// Keywords. (TOKEN_IDENTIFIER | TOKEN_KEYWORD)
	TOKEN_AND = 0x000C0001, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
	TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NULL,
	TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
	TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

	TOKEN_CONTROL = 0x00100000,
	TOKEN_MARKER,
	TOKEN_EOF,
	TOKEN_INVALID
};

union TokenValue {
	double number;
	String *string;
	char *identifier;
};

typedef struct Token {
	enum TokenType type;
	union TokenValue value;
	TextRange range;
} Token;


void Token_constructor(Token *token, enum TokenType type, union TokenValue value, TextRange range);
void Token_destructor(Token *token);
Token* Token_alloc(enum TokenType type, union TokenValue value, TextRange range);
void Token_free(Token *token);
void Token_print(Token *token, unsigned int depth, int isProperty);