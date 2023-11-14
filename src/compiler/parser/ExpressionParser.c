#include "compiler/parser/Parser.h"

#include <stdbool.h>

#include "assertf.h"
// #include "compiler/lexer/Lexer.h"
#include "compiler/parser/ExpressionParser.h"
#include "compiler/lexer/Token.h"
#include "internal/Array.h"
#include "allocator/MemoryAllocator.h"
#include "compiler/parser/ASTNodes.h"

#define TABLE_SIZE 9
#define STACK_SIZE 20

int precedence_table[TABLE_SIZE][TABLE_SIZE] = {   // [stack top terminal][input token]
	// +-|*/| ! |??|r |i |( |) |$
	{R, S, S, R, R, S, S, R, R}, // +-
	{R, R, S, R, R, S, S, R, R}, // */
	{R, R, X, R, R, X, X, R, R}, // !
	{S, S, S, S, S, S, S, R, R}, // ??
	{S, S, S, R, X, S, S, R, R}, // r (==, !=, <, >, <=, >=)
	{R, R, R, R, R, X, X, R, R}, // i
	{S, S, S, S, S, S, S, E, X}, // (
	{R, R, R, R, R, X, X, R, R}, // )
	{S, S, S, S, S, S, S, X, X}  // $
};

int Expr_getPrecTbIndex(Token *token) {
	if(!token){
		return I_DOLLAR;
	}
	switch(token->kind) {
		case TOKEN_PLUS:
		case TOKEN_MINUS:
			return I_ADDITIVE;

		case TOKEN_STAR:
		case TOKEN_SLASH:
			return I_MULTIPLICATIVE;

		case TOKEN_EXCLAMATION:
			return I_UNWRAP_OP;

		case TOKEN_NULL_COALESCING:
			return I_NIL_COALES;

		case TOKEN_EQUALITY:
		case TOKEN_NOT_EQUALITY:
		case TOKEN_LESS:
		case TOKEN_GREATER:
		case TOKEN_LESS_EQUAL:
		case TOKEN_GREATER_EQUAL:
			return I_REL_OP;

		case TOKEN_LEFT_BRACE:
			return I_LEFT_BRAC;

		case TOKEN_RIGHT_BRACE:
			return I_RIGHT_BRAC;

		case TOKEN_DEFAULT:
			if(token->type == TOKEN_IDENTIFIER) {
				return I_ID;
			}
			return I_DOLLAR;

		case TOKEN_STRING:
		case TOKEN_INTEGER:
		case TOKEN_FLOATING:
		case TOKEN_NIL:
			return I_ID;

		default:
			return I_DOLLAR;
	}
}

StackItem* Expr_getTopTerminal(Array *stack) {
	StackItem *top = NULL;

	for(size_t i = 0; i < stack->size; i++) {
		top = Array_get(stack, stack->size - i - 1);

		if(top->Stype == S_TERMINAL || top->Stype == S_BOTTOM) {
			return top;
		}
	}

	return top;
}

void Expr_pushAfterTopTerminal(Array *stack) {
	StackItem *stopReduction = mem_alloc(sizeof(StackItem));
	stopReduction->token = NULL;
	stopReduction->Stype = S_STOP;
	stopReduction->node = NULL;

	for(size_t i = 0; i < stack->size; i++) {
		StackItem *top = Array_get(stack, stack->size - i - 1);

		if(top->Stype == S_TERMINAL ||top->Stype == S_BOTTOM) {
			Array_insert(stack, (int)stack->size - i, stopReduction);
			return;
		}
	}
}

StackItem* Expr_performReduction(Array *stack) {

	// E -> i
	if(stack->size == 1) {
		StackItem *id = Array_pop(stack);

		if(id->Stype == S_TERMINAL) {
			if(id->token->type == TOKEN_LITERAL) {
				LiteralExpressionASTNode *literalE = new_LiteralExpressionASTNode(id->token->value);
				id->node = (ExpressionASTNode*)literalE;
				id->Stype = S_NONTERMINAL;

				return id;
			}

			if(id->token->type == TOKEN_IDENTIFIER) {
				IdentifierASTNode *identifierE = new_IdentifierASTNode(id->token->value.string); // string or identifier?
				id->node = (ExpressionASTNode*)identifierE;
				id->Stype = S_NONTERMINAL;

				return id;
			}
		}
		// two operators consecutively
		else {
			return NULL;
		}
	}

	// E -> E!
	if(stack->size == 2) {
		StackItem *argument = Array_pop(stack);
		StackItem *operator = Array_pop(stack);

		if(operator->token->kind == TOKEN_EXCLAMATION && argument->Stype == S_NONTERMINAL) {
			UnaryExpressionASTNode *unaryE = new_UnaryExpressionASTNode(argument->node, OPERATOR_UNWRAP);
			operator->node = (ExpressionASTNode*)unaryE;
			operator->Stype = S_NONTERMINAL;

			mem_free(argument);

			return operator;
		} else {
			return NULL;
		}
	}

	// Binary operations and parentheses
	if(stack->size == 3) {
		StackItem *leftOperand = Array_pop(stack);
		StackItem *operator = Array_pop(stack);
		StackItem *rightOperand = Array_pop(stack);

		// E -> (E)
		if(operator->Stype == S_NONTERMINAL && leftOperand->token->kind == TOKEN_LEFT_PAREN && rightOperand->token->kind == TOKEN_RIGHT_PAREN) {
			mem_free(leftOperand);
			mem_free(rightOperand);

			return operator;
		}

		enum OperatorType operatorType = 0;
		if(leftOperand->Stype == S_NONTERMINAL && rightOperand->Stype == S_NONTERMINAL)
			switch(operator->token->kind) {
				case TOKEN_PLUS:
					operatorType = OPERATOR_PLUS;
					break;
				case TOKEN_MINUS:
					operatorType = OPERATOR_MINUS;
					break;
				case TOKEN_STAR:
					operatorType = OPERATOR_MUL;
					break;
				case TOKEN_SLASH:
					operatorType = OPERATOR_DIV;
					break;
				case TOKEN_EQUALITY:
					operatorType = OPERATOR_EQUAL;
					break;
				case TOKEN_NOT_EQUALITY:
					operatorType = OPERATOR_NOT_EQUAL;
					break;
				case TOKEN_LESS:
					operatorType = OPERATOR_LESS;
					break;
				case TOKEN_GREATER:
					operatorType = OPERATOR_GREATER;
					break;
				case TOKEN_LESS_EQUAL:
					operatorType = OPERATOR_LESS_EQUAL;
					break;
				case TOKEN_GREATER_EQUAL:
					operatorType = OPERATOR_GREATER_EQUAL;
					break;
				case TOKEN_NULL_COALESCING:
					operatorType = OPERATOR_NULL_COALESCING;
					break;
				default:
					break;
			}
		if(operatorType) {
			BinaryExpressionASTNode *binaryE = new_BinaryExpressionASTNode(leftOperand->node, rightOperand->node, operatorType);
			operator->node = (ExpressionASTNode*)binaryE;
			operator->Stype = S_NONTERMINAL;

			mem_free(leftOperand);
			mem_free(rightOperand);

			return operator;
		}
	}

	return NULL;
}

bool Expr_Reduce(Array *stack, StackItem *currentToken) {
	Array *reduceStack = Array_alloc(STACK_SIZE);

	if(stack->size == 1){
		return false;
	}

	while((currentToken = Array_pop(stack))->Stype != S_STOP) {
		if(currentToken->Stype != S_STOP) {
			Array_push(reduceStack, currentToken);
		}
	}

	// Perform reduction and push result on stack (nonterminal)
	currentToken = Expr_performReduction(reduceStack);
	if(currentToken != NULL) {
		Array_push(stack, currentToken);
		return true;
	} else {
		return false;
	}
}

ParserResult __Parser_parseExpression(Parser *parser) {
	assertf(parser != NULL);

	Array *stack = Array_alloc(STACK_SIZE);
	// Array *reduceStack = Array_alloc(STACK_SIZE);
	// Token *token = NULL;
	StackItem *bottom = mem_alloc(sizeof(StackItem));

	bottom->Stype = S_BOTTOM;
	bottom->node = NULL;
	bottom->token = NULL;
	Array_push(stack, bottom);

	bool reductionSucces;
	int offset = 1;
	LexerResult current = Lexer_peekToken(parser->lexer, offset);
	LexerResult removeFromTokenStream;

	while(true) {
		if(!current.success) return LexerToParserError(current);

		StackItem *topTerminal = Expr_getTopTerminal(stack);
		// topTerminal returns S_BOTTOM, which has no token,
		// this token is being dereferenced in Expr_getPrecTbIndex, thus causing a segfault

		int topTerminalIndex = Expr_getPrecTbIndex(topTerminal->token);
		int currentTokenIndex = Expr_getPrecTbIndex(current.token);

		enum PrecTableRelation operation = precedence_table[topTerminalIndex][currentTokenIndex];

		StackItem *isItFinal = Array_get(stack, stack->size - 1);
		if(isItFinal->Stype == S_NONTERMINAL && stack->size == 2 && operation == X) {
			StackItem *finalExpression = Array_pop(stack);
			bottom = Array_pop(stack);
			mem_free(bottom);
			Array_free(stack);
			// Array_free(reduceStack);

			return ParserSuccess(finalExpression->node);
		}

		StackItem *currentToken = mem_alloc(sizeof(StackItem));

		switch(operation) {
			case S: {
				currentToken->Stype = S_TERMINAL;
				currentToken->token = current.token;
				currentToken->node = NULL;
				Expr_pushAfterTopTerminal(stack);
				Array_push(stack, currentToken);

				removeFromTokenStream = Lexer_nextToken(parser->lexer);
				if(!(removeFromTokenStream.success)) return LexerToParserError(current);
				current = Lexer_peekToken(parser->lexer, offset);
			} break;

			case R: {
				reductionSucces = Expr_Reduce(stack, currentToken);
				if(!reductionSucces) {
					return ParserError(String_fromFormat("Syntax error in expression"), Array_fromArgs(1, current.token));
				}
			} break;

			case E: {
				currentToken->Stype = S_TERMINAL;
				currentToken->token = current.token;
				currentToken->node = NULL;
				Array_push(stack, currentToken);

				removeFromTokenStream = Lexer_nextToken(parser->lexer);
				if(!(removeFromTokenStream.success)) return LexerToParserError(current);
				current = Lexer_peekToken(parser->lexer, offset);
			} break;

			case X: {
				reductionSucces = Expr_Reduce(stack, currentToken);
				if(!reductionSucces) {
					return ParserError(String_fromFormat("Syntax error in expression"), Array_fromArgs(1, current.token));
				}
			} break;

			default: {} break;
		}

	}
	return ParserNoMatch();
}
