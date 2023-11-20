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
	I_ADDITIVE,
	I_MULTIPLICATIVE,
	I_UNWRAP_OP,
	I_NIL_COALES,
	I_REL_OP,
	I_ID,
	I_LEFT_PAREN,
	I_RIGHT_PAREN,
	I_NOT,
	I_AND,
	I_OR,
	I_DOLLAR
};

typedef enum {
	S_BOTTOM,
	S_STOP,
	S_TERMINAL,
	S_NONTERMINAL
}StackItemType;

typedef enum{
	P_IS_PREFIX,
	P_IS_POSTFIX,
	P_UNRESOLVED,
}PrefixStatus;

typedef struct StackItem {
	Token *token;
	StackItemType Stype;
	ExpressionASTNode *node;
	PrefixStatus isPrefix;
} StackItem;

StackItem* Expr_getTopTerminal(Array *stack);
void Expr_pushAfterTopTerminal(Array *stack);
StackItem* Expr_performReduction(Array *stack);

#endif
