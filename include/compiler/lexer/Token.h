/**
 * @file include/compiler/lexer/Token.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "internal/String.h"
#include "internal/TextRange.h"

#ifndef TOKEN_H
#define TOKEN_H

enum TokenType {
	TOKEN_INVALID = 0,
	TOKEN_EOF = 1,
	TOKEN_CONTROL,  // Forgot what this is for :(
	TOKEN_MARKER,   // Marker in the source code (for error messages)
	TOKEN_STRING_INTERPOLATION_MARKER,

	TOKEN_LITERAL,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD,
	TOKEN_OPERATOR,
	TOKEN_PUNCTUATOR
};

enum TokenKind {
	TOKEN_DEFAULT = 0,

	// Literals
	// This has to be at this position in the enum
	// because it's being converted into another
	// enum with the same values.
	TOKEN_STRING,
	TOKEN_INTEGER,
	TOKEN_FLOATING,
	TOKEN_BOOLEAN,
	TOKEN_NIL,

	// String interpolation markers
	TOKEN_STRING_HEAD,
	TOKEN_STRING_SPAN,
	TOKEN_STRING_TAIL,

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

	// Compound assignment operators (+=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=, ==, !=, ===, !==)
	TOKEN_PLUS_ASSIGN, TOKEN_MINUS_ASSIGN,
	TOKEN_MULT_ASSIGN, TOKEN_DIV_ASSIGN, TOKEN_MOD_ASSIGN,
	TOKEN_BIT_AND_ASSIGN, TOKEN_BIT_OR_ASSIGN, TOKEN_BIT_XOR_ASSIGN,
	TOKEN_LEFT_SHIFT_ASSIGN, TOKEN_RIGHT_SHIFT_ASSIGN,
	TOKEN_EQUALITY, TOKEN_NOT_EQUALITY,
	TOKEN_GREATER_EQUAL, TOKEN_LESS_EQUAL,
	// TOKEN_IDENTITY, TOKEN_NOT_IDENTITY,

	// Other compound operators (++, --, <<, >>, &&, ||, ??, ?., ..., ..<)
	TOKEN_INCREMENT, TOKEN_DECREMENT,
	TOKEN_LEFT_SHIFT, TOKEN_RIGHT_SHIFT,
	TOKEN_LOG_AND, TOKEN_LOG_OR,
	TOKEN_NULL_COALESCING, // TOKEN_OPTIONAL_CHAINING,
	TOKEN_RANGE, TOKEN_HALF_OPEN_RANGE,

	// Keywords
	TOKEN_IF, TOKEN_ELSE, TOKEN_VAR, TOKEN_LET,
	TOKEN_WHILE, TOKEN_FOR, TOKEN_IN, TOKEN_FUNC,
	TOKEN_RETURN, TOKEN_BREAK, TOKEN_CONTINUE
};

// xNSLxNSL
// ^--^^--^
//    R   L
enum WhitespaceType {
	WHITESPACE_NONE = 0,                // No whitespace

	// WHITESPACE_LEFT_LIMIT = 1 << 0,  // BOF or EOF
	WHITESPACE_LEFT_SPACE = 1 << 1,     // Space, tab, vertical tab or multi-line comment on a single line
	WHITESPACE_LEFT_NEWLINE = 1 << 2,   // Line feed, carriage return, multi-line comment on multiple lines or single-line comment

	// WHITESPACE_RIGHT_LIMIT = 1 << 4, // BOF or EOF
	WHITESPACE_RIGHT_SPACE = 1 << 5,    // Space, tab, vertical tab or multi-line comment on a single line
	WHITESPACE_RIGHT_NEWLINE = 1 << 6,  // Line feed, carriage return, multi-line comment on multiple lines or single-line comment

	WHITESPACE_LEFT = WHITESPACE_LEFT_SPACE | WHITESPACE_LEFT_NEWLINE,
	WHITESPACE_RIGHT = WHITESPACE_RIGHT_SPACE | WHITESPACE_RIGHT_NEWLINE,
	WHITESPACE_ANY = WHITESPACE_LEFT | WHITESPACE_RIGHT,

	WHITESPACE_MASK_LEFT = /*WHITESPACE_LEFT_LIMIT |*/ WHITESPACE_LEFT_SPACE | WHITESPACE_LEFT_NEWLINE,
	WHITESPACE_MASK_RIGHT = /*WHITESPACE_RIGHT_LIMIT |*/ WHITESPACE_RIGHT_SPACE | WHITESPACE_RIGHT_NEWLINE
};

#define whitespace_left(whitespace) ((whitespace) & WHITESPACE_LEFT)
#define whitespace_right(whitespace) ((whitespace) & WHITESPACE_RIGHT)
#define whitespace_both(whitespace) (whitespace_left(whitespace) && whitespace_right(whitespace))
#define whitespace_none(whitespace) ((whitespace) == WHITESPACE_NONE)
#define whitespace_consistent(whitespace) (whitespace_both(whitespace) || whitespace_none(whitespace))

#define right_to_left_whitespace(whitespace) (((whitespace) & WHITESPACE_RIGHT) >> 4)
#define left_to_right_whitespace(whitespace) (((whitespace) & WHITESPACE_LEFT) << 4)

union TokenValue {
	double floating;
	long integer;
	unsigned int boolean;
	String *string;
	String *identifier;
};

typedef struct Token {
	enum TokenType type;
	enum TokenKind kind;
	enum WhitespaceType whitespace;
	TextRange range;
	union TokenValue value;
} Token;


/**
 * Constructs an instance of Token.
 * @param token
 * @param type
 * @param kind
 * @param whitespace
 * @param range
 * @param value
 */
void Token_constructor(Token *token, enum TokenType type, enum TokenKind kind, enum WhitespaceType whitespace, TextRange range, union TokenValue value);

/**
 * Destructs an instance of Token.
 * @param token
 */
void Token_destructor(Token *token);

/**
 * Allocates a new instance of Token.
 * @param type
 * @param kind
 * @param whitespace
 * @param range
 * @param value
 * @return New instance of Token
 */
Token* Token_alloc(enum TokenType type, enum TokenKind kind, enum WhitespaceType whitespace, TextRange range, union TokenValue value);

/**
 * Frees an instance of Token.
 * @param token
 */
void Token_free(Token *token);

/**
 * Stringifies a token contents.
 * @param token
 * @return char*
 */
char* Token_toString(Token *token);

void Token_print(Token *token, unsigned int depth, int isProperty);

#endif

/** End of file include/compiler/lexer/Token.h **/
