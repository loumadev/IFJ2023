#include "compiler/lexer/Token.h"
#include "compiler/parser/ASTNodes.h"

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

enum PrecTableRelation {
	S, // Shift
	R, // Reduce
	E, // Equal
	X // Error
};

enum PrecTableIndex {
	I_PLUS_MINUS,
	I_MUL_DIV,
	I_EXCL_OP,
	I_NIL_COALES,
	I_REL_OP,
	I_ID,
	I_LEFT_BRAC,
	I_RIGHT_BRAC,
	I_DOLLAR
};

typedef enum {
	S_BOTTOM,
	S_STOP,
	S_TERMINAL,
	S_NONTERMINAL
}StackItemType;

typedef struct StackItem {
	Token *token;
	StackItemType Stype;
	ExpressionASTNode *node;
} StackItem;

#endif


//LexerResult current = Lexer_nextToken(parser->lexer);