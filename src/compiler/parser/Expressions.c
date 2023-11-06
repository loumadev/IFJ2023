#include "compiler/parser/Parser.h"

#include <stdbool.h>

#include "assertf.h"
//#include "internal/Array.h"
//#include "compiler/parser/ASTNodes.h"
//#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Expressions.h"
#include "compiler/lexer/Token.h"
#include "internal/Array.h"
#include "allocator/MemoryAllocator.h"
#include "compiler/parser/ASTNodes.h"

#define TABLE_SIZE 9
#define STACK_SIZE 20

int precedence_table[TABLE_SIZE][TABLE_SIZE] =    //[stack top terminal][input token]
{
  // +-|*/| ! |??|r |i |( |) |$
	{R, S, S, R, R, S, S, R, R}, // +-
	{R, R, S, R, R, S, S, R, R}, // */
	{R, R, R, R, R, S, S, R, R}, // !
	{S, S, S, S, S, S, S, R, R}, // ??
	{S, S, S, R, R, S, S, R, R}, // r (==, !=, <, >, <=, >=)
	{R, R, R, R, R, X, X, R, R}, // i
	{S, S, S, S, S, S, S, E, X}, // (
	{R, R, R, R, R, X, X, R, R}, // )
	{S, S, S, S, S, S, S, X, X} // $

};

int Expr_getPrecTbIndex(Token *token) {

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
			} //else error
			return I_DOLLAR; //maybe
		case TOKEN_STRING:
		case TOKEN_INTEGER:
		case TOKEN_FLOATING:
		case TOKEN_NIL:
			return I_ID;

		default:
			return I_DOLLAR;
	}
}

StackItem Expr_getTopTerminal(Array *stack) {
	StackItem *top = NULL;
	for(size_t i = 0; i < stack->size; i++) {
		if((top = Array_get(stack, stack->size - i - 1))->Stype == S_TERMINAL || top->Stype == S_BOTTOM) {
			return *top;
		}
	}
	return *top;
}

void Expr_pushAfterTopTerminal(Array *stack) {
	StackItem *stopReduction = mem_alloc(sizeof(StackItem));
	stopReduction->token = NULL;
	stopReduction->Stype = S_STOP;
	stopReduction->node = NULL;
	for(size_t i = 0; i < stack->size; i++) {
		if (((StackItem *)Array_get(stack, stack->size - i - 1))->Stype == S_TERMINAL || ((StackItem *)Array_get(stack, stack->size - i - 1))->Stype == S_BOTTOM) {
			Array_insert(stack, (int)stack->size - i, stopReduction);
			return;
		}
	}
}

StackItem *Expr_performReduction(Array *stack) {

	// E -> i
	if(stack->size == 1) {
		//StackItem *id = Array_get(stack, 0);
		StackItem *id = Array_pop(stack);

		if(id->Stype == S_TERMINAL){
			if(id->token->type == TOKEN_LITERAL) {
				LiteralExpressionASTNode *literalE = new_LiteralExpressionASTNode(id->token->value);
				id->node = (ExpressionASTNode*)literalE;
				id->Stype = S_NONTERMINAL;
				return id;
			}
			if(id->token->type == TOKEN_IDENTIFIER) {
				IdentifierASTNode *identifierE = new_IdentifierASTNode(id->token->value.string); //string or identifier?
				id->node = (ExpressionASTNode*)identifierE;
				id->Stype = S_NONTERMINAL;
				return id;
			}
		}
		//two operators consecutively
		else {return NULL;} //TODO: check for null in main
	}

	// E -> E!
	if(stack->size == 2) {
		//StackItem *operator = Array_get(stack, 1);
		//StackItem *argument = Array_get(stack, 0);
		StackItem *operator = Array_pop(stack);
		StackItem *argument = Array_pop(stack);

		if(operator->token->kind == TOKEN_EXCLAMATION && argument->Stype == S_NONTERMINAL) {
			UnaryExpressionASTNode *unaryE = new_UnaryExpressionASTNode(argument->node, OPERATOR_UNWRAP);
			operator->node = (ExpressionASTNode*)unaryE;
			operator->Stype = S_NONTERMINAL;
			return operator;
		}
		else {return NULL;}
	}

	// Binary operations and parentheses
	if(stack->size == 3) {
		//StackItem *operator = Array_get(stack, 1);
		//StackItem *leftOperand = Array_get(stack, 0);
		//StackItem *rightOperand = Array_get(stack, 2);
		StackItem *rightOperand = Array_pop(stack);
		StackItem *operator = Array_pop(stack);
		StackItem *leftOperand = Array_pop(stack);

		// E -> (E)
		if(operator->Stype == S_NONTERMINAL && leftOperand->token->kind == TOKEN_LEFT_PAREN && rightOperand->token->kind == TOKEN_RIGHT_PAREN) {
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
			return operator;
		}
	}
	return NULL; 
}

// TODO: function to determine whether to end or not

ParserResult __Parser_parseExpression(Parser *parser) {
	assertf(parser != NULL);

	Array *stack = Array_alloc(STACK_SIZE);
	Array *reduceStack = Array_alloc(STACK_SIZE);
	//Token *token = NULL;
	StackItem *bottom = mem_alloc(sizeof(StackItem));

	bottom->Stype = S_BOTTOM;
	bottom->node = NULL;
	Array_push(stack, bottom);

	LexerResult current = Lexer_nextToken(parser->lexer);

	while(true) {

		if(!current.success) return LexerToParserError(current);
		enum PrecTableRelation operation = precedence_table[Expr_getPrecTbIndex(Expr_getTopTerminal(stack).token)][Expr_getPrecTbIndex(current.token)];
		if(((StackItem*)Array_get(stack, stack->size - 1))->Stype == S_NONTERMINAL && stack->size == 2 && operation == X) {
			StackItem *finalExpression = Array_get(stack, stack->size);
			return ParserSuccess(finalExpression->node);
		} //TODO: when to end
		StackItem *shiftToken = mem_alloc(sizeof(StackItem));
		StackItem *reduceToken = mem_alloc(sizeof(StackItem));
		StackItem *equalsToken = mem_alloc(sizeof(StackItem));
		switch(operation) {
			case S:
				shiftToken->Stype = S_TERMINAL;
				shiftToken->token = current.token;
				shiftToken->node = NULL;
				Expr_pushAfterTopTerminal(stack);
				Array_push(stack, shiftToken);
				current = Lexer_nextToken(parser->lexer); //better check first with peekToken
				break;
			case R:
				while((reduceToken = Array_pop(stack))->Stype != S_STOP) {
					if(reduceToken->Stype != S_STOP) {
						Array_push(reduceStack, reduceToken);
					}
				}
				// Perform reduction and push result on stack (nonterminal)
				if(reduceToken == Expr_performReduction(reduceStack)){
					Array_push(stack, reduceToken);
				}
				else{
					return ParserError(String_fromFormat("TODO"), NULL);
					}
				break;
			case E:
				equalsToken->Stype = S_TERMINAL;
				equalsToken->token = current.token;
				Array_push(stack, equalsToken);
				break;
			case X:
				return ParserError(String_fromFormat("TODO"), NULL);
			default:
				break;


		}
	}

	return ParserNoMatch();
}