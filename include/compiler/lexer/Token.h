#include "internal/String.h"
#include "internal/TextRange.h"

enum TokenType {
	TOKEN_INVALID = 0,
	TOKEN_EOF = 1,
	TOKEN_CONTROL,  // Forgot what this is for :(
	TOKEN_MARKER,   // Marker in the source code (for error messages)

	TOKEN_LITERAL,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD,
	TOKEN_PUNCTUATION
};

enum TokenKind {
	TOKEN_DEFAULT = 0,

	// Literals
	TOKEN_STRING,
	TOKEN_INTEGER,
	TOKEN_FLOATING,
	TOKEN_BOOLEAN,
	TOKEN_NIL,

	// Single-character tokens
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
	TOKEN_COMMA, TOKEN_DOT, TOKEN_COLON, TOKEN_SEMICOLON,
	TOKEN_MINUS, TOKEN_PLUS, TOKEN_SLASH, TOKEN_ASTERISK,
	TOKEN_EXCLAMATION, TOKEN_QUESTION, TOKEN_PERCENT,
	TOKEN_CARET /*valid marker*/, TOKEN_TILDE /*valid marker*/, TOKEN_AMPERSAND, TOKEN_PIPE,

	TOKEN_BANG, TOKEN_BANG_EQUAL,
	TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
	TOKEN_GREATER, TOKEN_GREATER_EQUAL,
	TOKEN_LESS, TOKEN_LESS_EQUAL,

	// Keywords
	TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
	TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NULL,
	TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
	TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE
};

union TokenValue {
	double floating;
	long integer;
	String *string;
	String *identifier;
};

typedef struct Token {
	enum TokenType type;
	enum TokenKind kind;
	TextRange range;
	union TokenValue value;
} Token;


void Token_constructor(Token *token, enum TokenType type, enum TokenKind kind, TextRange range, union TokenValue value);
void Token_destructor(Token *token);
Token* Token_alloc(enum TokenType type, enum TokenKind kind, TextRange range, union TokenValue value);
void Token_free(Token *token);
void Token_print(Token *token, unsigned int depth, int isProperty);
