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
	TOKEN_OPERATOR,
	TOKEN_PUNCTUATOR
};

enum TokenKind {
	TOKEN_DEFAULT = 0,

	// Literals
	TOKEN_STRING,
	TOKEN_INTEGER,
	TOKEN_FLOATING,
	TOKEN_BOOLEAN,
	TOKEN_NIL,

	// Punctuators ((, ), {, }, [, ], ., ,, :, ;, =, @, #, & (as a prefix operator), ->, `, ?, and ! (as a postfix operator))
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
	TOKEN_DOT, TOKEN_COMMA, TOKEN_COLON, TOKEN_SEMICOLON,
	TOKEN_EQUAL, TOKEN_AT, TOKEN_HASH, TOKEN_AMPERSAND, TOKEN_ARROW,
	TOKEN_BACKTICK, TOKEN_QUESTION, TOKEN_EXCLAMATION,

	// Operators (/, =, -, +, !, *, %, <, >, &, |, ^, ?, or ~)
	TOKEN_SLASH, TOKEN_MINUS, TOKEN_PLUS,
	TOKEN_STAR, TOKEN_PERCENT, TOKEN_LESS, TOKEN_GREATER,
	TOKEN_PIPE, TOKEN_CARET /*valid marker*/, TOKEN_TILDE /*valid marker*/,

	// Compound assignment operators (+=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=)
	TOKEN_PLUS_ASSIGN, TOKEN_MINUS_ASSIGN,
	TOKEN_MULT_ASSIGN, TOKEN_DIV_ASSIGN, TOKEN_MOD_ASSIGN,
	TOKEN_BIT_AND_ASSIGN, TOKEN_BIT_OR_ASSIGN, TOKEN_BIT_XOR_ASSIGN,
	TOKEN_LEFT_SHIFT_ASSIGN, TOKEN_RIGHT_SHIFT_ASSIGN,

	// Other compound operators (++, --, <<, >>, &&, ||, ??, ..., ..<)
	TOKEN_INCREMENT, TOKEN_DECREMENT,
	TOKEN_LEFT_SHIFT, TOKEN_RIGHT_SHIFT,
	TOKEN_LOG_AND, TOKEN_LOG_OR,
	TOKEN_NULL_COALESCING,
	TOKEN_RANGE, TOKEN_HALF_OPEN_RANGE,

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
