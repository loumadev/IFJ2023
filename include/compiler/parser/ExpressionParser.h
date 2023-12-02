/**
 * @file include/compiler/parser/ExpressionParser.h
 * @author Author Name <xlogin00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/lexer/Token.h"
#include "compiler/parser/ASTNodes.h"
#include "compiler/parser/Parser.h"

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

enum PrecTableRelation {
	S, // Shift
	R, // Reduce
	E, // Equal
	X  // Error
};

enum PrecTableIndex {
	I_ADDITIVE,          // +,-
	I_MULTIPLICATIVE,    // *,/
	I_UNWRAP_OP,		 // x!
	I_NIL_COALES,	     // ??
	I_REL_OP,            // ==, !=, <, >, <=, >=
	I_ID,				 // i
	I_LEFT_PAREN,		 // (
	I_RIGHT_PAREN,       // )
	I_NOT,               // !x
	I_AND,				 // &&
	I_OR,				 // ||
	I_DOLLAR             // $
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

enum PrecTableIndex Expr_getPrecTbIndex(Token *token, bool isIdentifier, Parser *parser, PrefixStatus status);

/**
 * Gets the top terminal from the stack.
 *
 * @param stack The stack of tokens.
 * @return The top terminal.
 */
StackItem* Expr_getTopTerminal(Array *stack);

/**
 * Pushes a stop reduction item after the top terminal on the stack.
 *
 * @param stack The stack of tokens.
 */
void Expr_pushAfterTopTerminal(Array *stack);

/**
 * Performs a reduction operation on the stack.
 *
 * @param stack The stack of tokens to perform reduction on.
 * @return The resulting item of the reduction.
 */
StackItem* Expr_performReduction(Array *stack);

/**
 * Selects items from stack to be reduced.
 *
 * @param stack The stack of tokens.
 * @param currentToken The token instance.
 * @return True if reduction was successful, false otherwise.
 */
bool Expr_Reduce(Array *stack, StackItem *currentToken);

/**
 * Parses an expression using the precedence climbing method.
 *
 * @param parser The parser instance.
 * @return The result of the expression parsing.
 */
ParserResult __Parser_parseExpression(Parser *parser);

#endif

/** End of file include/compiler/parser/ExpressionParser.h **/
