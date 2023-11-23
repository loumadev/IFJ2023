#include "compiler/parser/ExpressionParser.h"

#include <stdbool.h>

#include "assertf.h"
// #include "compiler/lexer/Lexer.h"
#include "allocator/MemoryAllocator.h"
#include "internal/Array.h"
#include "compiler/lexer/Token.h"
#include "compiler/parser/Parser.h"
#include "compiler/parser/ASTNodes.h"

#define TABLE_SIZE 12
#define STACK_SIZE 20

ParserResult __Parser_parseFunctionCallExpression(Parser *parser);
ParserResult __Parser_parseStringInterpolation(Parser *parser);
PrefixStatus prefix = P_UNRESOLVED;

int precedence_table[TABLE_SIZE][TABLE_SIZE] = {   // [stack top terminal][input token]
	// +-|*/| x!|??|r |i |( |)| !x||||&&|$
	{R, S, S, R, R, S, S, R, S, R, R, R}, // +-
	{R, R, S, R, R, S, S, R, S, R, R, R}, // */
	{R, R, X, R, R, X, X, R, X, R, R, R}, // x!
	{S, S, S, S, S, S, S, R, S, S, S, R}, // ??
	{S, S, S, R, X, S, S, R, S, R, R, R}, // r (==, !=, <, >, <=, >=)
	{R, R, R, R, R, X, X, R, X, R, R, R}, // i
	{S, S, S, S, S, S, S, E, S, S, S, X}, // (
	{R, R, R, R, R, X, X, R, X, R, R, R}, // )
	{R, R, R, R, R, S, X, R, X, R, R, R}, // !x
	{S, S, S, S, S, S, S, R, S, R, R, R}, // ||
	{S, S, S, S, S, S, S, R, S, R, R, R}, // &&
	{S, S, S, S, S, S, S, X, S, S, S, X}  // $
};

int Expr_getPrecTbIndex(Token *token, bool isIdentifier, Parser *parser, PrefixStatus status) {
	prefix = P_UNRESOLVED;
	if(!token) {
		if(isIdentifier) {
			return I_ID;
		}
		return I_DOLLAR;
	}
	LexerResult postfixPrefix;
	switch(token->kind) {
		case TOKEN_PLUS:
		case TOKEN_MINUS:
			return I_ADDITIVE;

		case TOKEN_STAR:
		case TOKEN_SLASH:
			return I_MULTIPLICATIVE;

		case TOKEN_EXCLAMATION:
			if(status == P_IS_POSTFIX) {
				return I_UNWRAP_OP;
			}
			if(status == P_IS_PREFIX) {
				return I_NOT;
			}
			// x!
			if(!whitespace_left(token->whitespace)) {
				postfixPrefix = Lexer_peekToken(parser->lexer, 0);
				if(postfixPrefix.success) {
					if((postfixPrefix.token->type == TOKEN_IDENTIFIER) ||
					   (postfixPrefix.token->type == TOKEN_LITERAL)) {
						prefix = P_IS_POSTFIX;
						return I_UNWRAP_OP;
					}
				}
			}

			// !x
			if(!whitespace_right(token->whitespace)) {
				postfixPrefix = Lexer_peekToken(parser->lexer, 2);
				if(postfixPrefix.success) {
					if((postfixPrefix.token->type == TOKEN_IDENTIFIER) ||
					   (postfixPrefix.token->type == TOKEN_LITERAL)) {
						prefix = P_IS_PREFIX;
						return I_NOT;
					}
				}
			}
			return I_DOLLAR;

		case TOKEN_NULL_COALESCING:
			return I_NIL_COALES;

		case TOKEN_EQUALITY:
		case TOKEN_NOT_EQUALITY:
		case TOKEN_LESS:
		case TOKEN_GREATER:
		case TOKEN_LESS_EQUAL:
		case TOKEN_GREATER_EQUAL:
			return I_REL_OP;

		case TOKEN_LEFT_PAREN:
			return I_LEFT_PAREN;

		case TOKEN_RIGHT_PAREN:
			return I_RIGHT_PAREN;

		case TOKEN_DEFAULT:
			if(token->type == TOKEN_IDENTIFIER) {
				return I_ID;
			}
			return I_DOLLAR;

		case TOKEN_STRING:
		case TOKEN_INTEGER:
		case TOKEN_FLOATING:
		case TOKEN_NIL:
		case TOKEN_BOOLEAN:
			return I_ID;

		case TOKEN_LOG_OR:
			return I_OR;

		case TOKEN_LOG_AND:
			return I_AND;

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

		if(top->Stype == S_TERMINAL || top->Stype == S_BOTTOM) {
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
			// function call
			if((id->token == NULL) && (id->node != NULL)) {
				id->node = (ExpressionASTNode*)id->node;
				id->Stype = S_NONTERMINAL;

				return id;
			}

			if(id->token->type == TOKEN_LITERAL) {
				LiteralExpressionASTNode *literalE = new_LiteralExpressionASTNode(Analyser_getTypeFromToken(id->token->kind), id->token->value);
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

	// E -> E! or E -> !E
	if(stack->size == 2) {
		StackItem *argument = Array_pop(stack);
		StackItem *operator = Array_pop(stack);

		if(operator->token->kind == TOKEN_EXCLAMATION && argument->Stype == S_NONTERMINAL) {
			UnaryExpressionASTNode *unaryE = new_UnaryExpressionASTNode(argument->node, OPERATOR_UNWRAP, false);
			operator->node = (ExpressionASTNode*)unaryE;
			operator->Stype = S_NONTERMINAL;

			mem_free(argument);

			return operator;
		} else if(operator->Stype == S_NONTERMINAL && argument->token->kind == TOKEN_EXCLAMATION) {
			UnaryExpressionASTNode *unaryLogE = new_UnaryExpressionASTNode(operator->node, OPERATOR_NOT, true);
			argument->node = (ExpressionASTNode*)unaryLogE;
			argument->Stype = S_NONTERMINAL;

			mem_free(operator);

			return argument;
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
				case TOKEN_LOG_OR:
					operatorType = OPERATOR_OR;
					break;
				case TOKEN_LOG_AND:
					operatorType = OPERATOR_AND;
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

	if(stack->size == 1) {
		return false;
	}

	while((currentToken = Array_pop(stack))->Stype != S_STOP) {
		if(currentToken->Stype != S_STOP) {
			Array_push(reduceStack, currentToken);
		}
	}

	// Perform reduction and push result on stack (nonterminal)
	currentToken = Expr_performReduction(reduceStack);
	Array_free(reduceStack);

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
	StackItem *bottom = mem_alloc(sizeof(StackItem));

	bottom->Stype = S_BOTTOM;
	bottom->node = NULL;
	bottom->token = NULL;
	bottom->isPrefix = P_UNRESOLVED;
	Array_push(stack, bottom);

	bool reductionSuccess;
	bool isIdentifier = false;
	int offset = 1;
	enum PrecTableRelation operation;

	LexerResult current = Lexer_peekToken(parser->lexer, offset);

	while(true) {
		if(!current.success) return LexerToParserError(current);
		isIdentifier = false;

		// check if there is a function call in expression
		if(current.token->type == TOKEN_IDENTIFIER) {
			// Check for '_' identifier
			if(String_equals(current.token->value.string, "_")) {
				return ParserError(
					String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
					Array_fromArgs(1, current.token)
				);
			}

			LexerResult next = Lexer_peekToken(parser->lexer, offset + 1);
			if(!next.success) return LexerToParserError(current);

			if(next.token->kind == TOKEN_LEFT_PAREN) {
				ParserResult functionCallExpression = __Parser_parseFunctionCallExpression(parser);
				if(!functionCallExpression.success) return functionCallExpression;

				isIdentifier = true;

				StackItem *function = mem_alloc(sizeof(StackItem));
				function->node = functionCallExpression.node;
				function->Stype = S_TERMINAL;
				function->token = NULL;
				function->isPrefix = P_UNRESOLVED;
				Expr_pushAfterTopTerminal(stack);
				Array_push(stack, function);

				current = Lexer_peekToken(parser->lexer, offset);
				if(!current.success) return LexerToParserError(current);
			}
		}

		// check for string interpolation
		if(current.token->kind == TOKEN_STRING) {
			LexerResult next = Lexer_peekToken(parser->lexer, offset + 1);
			if(!next.success) return LexerToParserError(current);

			if(next.token->kind == TOKEN_STRING_HEAD) {
				ParserResult stringInterpolation = __Parser_parseStringInterpolation(parser);
				if(!stringInterpolation.success) return stringInterpolation;

				isIdentifier = true;

				StackItem *string = mem_alloc(sizeof(StackItem));
				string->node = stringInterpolation.node;
				string->Stype = S_TERMINAL;
				string->token = NULL;
				string->isPrefix = P_UNRESOLVED;
				Expr_pushAfterTopTerminal(stack);
				Array_push(stack, string);

				current = Lexer_peekToken(parser->lexer, offset);
				if(!current.success) return LexerToParserError(current);
			}
		}

		if(operation != X) {
			StackItem *topTerminal = Expr_getTopTerminal(stack);

			int topTerminalIndex = Expr_getPrecTbIndex(topTerminal->token, isIdentifier, parser, topTerminal->isPrefix);
			int currentTokenIndex = Expr_getPrecTbIndex(current.token, false, parser, P_UNRESOLVED);

			operation = precedence_table[topTerminalIndex][currentTokenIndex];
		}

		StackItem *isItFinal = Array_get(stack, stack->size - 1);
		if(isItFinal->Stype == S_NONTERMINAL && stack->size == 2 && operation == X) {
			StackItem *finalExpression = Array_pop(stack);
			bottom = Array_pop(stack);
			mem_free(bottom);
			Array_free(stack);

			return ParserSuccess(finalExpression->node);
		}

		StackItem *currentToken = mem_alloc(sizeof(StackItem));

		switch(operation) {
			case S: {
				currentToken->Stype = S_TERMINAL;
				currentToken->token = current.token;
				currentToken->node = NULL;
				if(prefix == P_IS_POSTFIX) {
					currentToken->isPrefix = P_IS_POSTFIX;
				}
				if(prefix == P_IS_PREFIX) {
					currentToken->isPrefix = P_IS_PREFIX;
				}
				if(prefix == P_UNRESOLVED) {
					currentToken->isPrefix = P_UNRESOLVED;
				}
				Expr_pushAfterTopTerminal(stack);
				Array_push(stack, currentToken);

				LexerResult removeFromTokenStream = Lexer_nextToken(parser->lexer);
				if(!(removeFromTokenStream.success)) return LexerToParserError(current);
				current = Lexer_peekToken(parser->lexer, offset);
			} break;

			case R: {
				reductionSuccess = Expr_Reduce(stack, currentToken);
				if(!reductionSuccess) {
					return ParserError(String_fromFormat("Syntax error in expression"), Array_fromArgs(1, current.token));
				}
			} break;

			case E: {
				currentToken->Stype = S_TERMINAL;
				currentToken->token = current.token;
				currentToken->node = NULL;
				Array_push(stack, currentToken);

				LexerResult removeFromTokenStream = Lexer_nextToken(parser->lexer);
				if(!(removeFromTokenStream.success)) return LexerToParserError(current);
				current = Lexer_peekToken(parser->lexer, offset);
			} break;

			case X: {
				reductionSuccess = Expr_Reduce(stack, currentToken);
				if(!reductionSuccess) {
					return ParserError(String_fromFormat("Syntax error in expression"), Array_fromArgs(1, current.token));
				}
			} break;

			default: {} break;
		}

	}
	return ParserNoMatch();
}
