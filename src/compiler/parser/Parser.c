/**
 * @file src/compiler/parser/Parser.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/parser/Parser.h"

#include <stdbool.h>

#include "assertf.h"
#include "internal/Array.h"
#include "compiler/parser/ASTNodes.h"
#include "compiler/lexer/Lexer.h"


/* Register private functions */

ParserResult __Parser_parseProgram(Parser *parser);
ParserResult __Parser_parseBlock(Parser *parser, bool requireBraces);
ParserResult __Parser_parseStatement(Parser *parser);
ParserResult __Parser_parseExpression(Parser *parser);
ParserResult __Parser_parseTypeReference(Parser *parser);
ParserResult __Parser_parseParameter(Parser *parser);
ParserResult __Parser_parseParameterList(Parser *parser);
ParserResult __Parser_parseFuncStatement(Parser *parser);
ParserResult __Parser_parsePattern(Parser *parser);
ParserResult __Parser_parseOptionalBindingCondition(Parser *parser);
ParserResult __Parser_parseTest(Parser *parser);
ParserResult __Parser_parseIfStatement(Parser *parser);
ParserResult __Parser_parseWhileStatement(Parser *parser);
ParserResult __Parser_parseReturnStatement(Parser *parser);
ParserResult __Parser_parseVariableDeclarator(Parser *parser);
ParserResult __Parser_parseVariableDeclarationList(Parser *parser);
ParserResult __Parser_parseVariableDeclarationStatement(Parser *parser);
ParserResult __Parser_parseArgument(Parser *parser);
ParserResult __Parser_parseArgumentList(Parser *parser);
ParserResult __Parser_parseFunctionCallExpression(Parser *parser);
ParserResult __Parser_parseAssignmentStatement(Parser *parser);
ParserResult __Parser_parseRange(Parser *parser);
ParserResult __Parser_parseForStatement(Parser *parser);
ParserResult __Parser_parseBreakStatement(Parser *parser);
ParserResult __Parser_parseContinueStatement(Parser *parser);

/* Definitions of public functions */

void Parser_constructor(Parser *parser, Lexer *lexer) {
	assertf(parser != NULL);
	assertf(lexer != NULL);

	parser->lexer = lexer;
	parser->lastLexerError = LexerErrorCustom(RESULT_INVALID, NULL, NULL);
}

void Parser_destructor(Parser *parser) {
	parser->lexer = NULL;
}

void Parser_setLexer(Parser *parser, Lexer *lexer) {
	parser->lexer = lexer;
}

bool Parser_hasLexerError(Parser *parser) {
	assertf(parser != NULL);
	assertf(parser->lexer != NULL);

	return parser->lastLexerError.type != RESULT_INVALID;
}

LexerResult Parser_flushLastLexerError(Parser *parser) {
	assertf(parser != NULL);
	assertf(parser->lexer != NULL);
	assertf(Parser_hasLexerError(parser), "Cannot flush lexer error if there is none");

	// Save the error
	LexerResult result = parser->lastLexerError;

	// Reset the error
	parser->lastLexerError = LexerErrorCustom(RESULT_INVALID, NULL, NULL);

	// Return the saved error
	return result;
}

bool Parser_isAtEnd(Parser *parser) {
	assertf(parser != NULL);
	assertf(parser->lexer != NULL);

	LexerResult result = Lexer_peekToken(parser->lexer, 1);
	if(!result.success) {
		parser->lastLexerError = result;
		return false;
	}

	return result.token->type == TOKEN_EOF;
}

ParserResult Parser_parse(Parser *parser) {
	assertf(parser != NULL);
	assertf(parser->lexer != NULL);

	return __Parser_parseProgram(parser);
}


/* Definitions of private functions */

ParserResult __Parser_parseProgram(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	ParserResult result = __Parser_parseBlock(parser, false);
	if(!result.success) return result;

	FLUSH_ERROR_BUFFER(parser);

	ProgramASTNode *program = new_ProgramASTNode((BlockASTNode*)result.node);
	return ParserSuccess(program);
}

ParserResult __Parser_parseBlock(Parser *parser, bool requireBraces) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for left brace
	if(requireBraces) {
		LexerResult result = Lexer_nextToken(parser->lexer);
		if(!result.success) return LexerToParserError(result);

		if(result.token->kind != TOKEN_LEFT_BRACE) {
			return ParserError(
				String_fromFormat("expected '{' in block body, but got '%s'", Token_toString(result.token)),
				Array_fromArgs(1, result.token)
			);
		}
	}

	// Parse statements
	Array *statements = Array_alloc(0);

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	while(!Parser_isAtEnd(parser) && (!requireBraces || (requireBraces && peek.token->kind != TOKEN_RIGHT_BRACE))) {
		ParserResult result = __Parser_parseStatement(parser);
		if(!result.success) return result;

		if(result.type == RESULT_NO_MATCH) {
			return ParserError(
				String_fromFormat("expected '}' in block body, but got '%s'", Token_toString(peek.token)),
				Array_fromArgs(1, peek.token)
			);
		}

		Array_push(statements, result.node);

		// Check for delimiter after statement
		peek = Lexer_peekToken(parser->lexer, 1);
		if(!peek.success) return LexerToParserError(peek);

		// They don't want us to have semicolons :(
		if(peek.token->kind == TOKEN_SEMICOLON) {
			return ParserError(
				String_fromFormat("';' is not supported after statement, use new line instead"),
				Array_fromArgs(1, peek.token)
			);
		} else if(!(peek.token->whitespace & WHITESPACE_LEFT_NEWLINE) && !Parser_isAtEnd(parser)) {
			if((requireBraces && peek.token->kind != TOKEN_RIGHT_BRACE) || !requireBraces) {
				return ParserError(
					String_fromFormat("expected new line after statement"),
					Array_fromArgs(1, peek.token)
				);
			}
		}
	}

	// Check for right brace
	if(requireBraces) {
		LexerResult result = Lexer_nextToken(parser->lexer);
		if(!result.success) return LexerToParserError(result);

		if(result.token->kind != TOKEN_RIGHT_BRACE) {
			return ParserError(
				String_fromFormat("expected '}' in block body, but got '%s'", Token_toString(result.token)),
				Array_fromArgs(1, result.token)
			);
		}
	}

	BlockASTNode *block = new_BlockASTNode(statements);
	return ParserSuccess(block);
}

ParserResult __Parser_parseStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	if(peek.token->kind == TOKEN_FUNC) {
		ParserResult funcResult = __Parser_parseFuncStatement(parser);
		if(!funcResult.success) return funcResult;
		return ParserSuccess(funcResult.node);
	}

	if(peek.token->kind == TOKEN_IF) {
		ParserResult ifResult = __Parser_parseIfStatement(parser);
		if(!ifResult.success) return ifResult;
		return ParserSuccess(ifResult.node);
	}

	if(peek.token->kind == TOKEN_WHILE) {
		ParserResult whileResult = __Parser_parseWhileStatement(parser);
		if(!whileResult.success) return whileResult;
		return ParserSuccess(whileResult.node);
	}

	if(peek.token->kind == TOKEN_FOR) {
		ParserResult forResult = __Parser_parseForStatement(parser);
		if(!forResult.success) return forResult;
		return ParserSuccess(forResult.node);
	}

	if(peek.token->kind == TOKEN_RETURN) {
		ParserResult returnResult = __Parser_parseReturnStatement(parser);
		if(!returnResult.success) return returnResult;
		return ParserSuccess(returnResult.node);
	}

	if(peek.token->kind == TOKEN_BREAK) {
		ParserResult breakResult = __Parser_parseBreakStatement(parser);
		if(!breakResult.success) return breakResult;
		return ParserSuccess(breakResult.node);
	}

	if(peek.token->kind == TOKEN_CONTINUE) {
		ParserResult continueResult = __Parser_parseContinueStatement(parser);
		if(!continueResult.success) return continueResult;
		return ParserSuccess(continueResult.node);
	}

	if(peek.token->kind == TOKEN_LET || peek.token->kind == TOKEN_VAR) {
		ParserResult variableDeclarationResult = __Parser_parseVariableDeclarationStatement(parser);
		if(!variableDeclarationResult.success) return variableDeclarationResult;
		return ParserSuccess(variableDeclarationResult.node);
	}

	if(peek.token->type == TOKEN_IDENTIFIER) {
		LexerResult tmp = Lexer_peekToken(parser->lexer, 2);
		if(!tmp.success) return LexerToParserError(tmp);

		if(tmp.token->kind == TOKEN_EQUAL) {
			ParserResult assignmentStatementResult = __Parser_parseAssignmentStatement(parser);
			if(!assignmentStatementResult.success) return assignmentStatementResult;
			return ParserSuccess(assignmentStatementResult.node);
		} else if(tmp.token->kind == TOKEN_LEFT_PAREN) {
			// Check for '_' identifier
			if(String_equals(peek.token->value.string, "_")) {
				return ParserError(
					String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
					Array_fromArgs(1, peek.token)
				);
			}

			// function call
			ParserResult functionCallExpression = __Parser_parseFunctionCallExpression(parser);
			if(!functionCallExpression.success) return functionCallExpression;
			ExpressionStatementASTNode *expressionStatement = new_ExpressionStatementASTNode((ExpressionASTNode*)functionCallExpression.node);
			return ParserSuccess(expressionStatement);
		}

	}

	return ParserNoMatch();
}

// ParserResult __Parser_parseExpression(Parser *parser) {
//	assertf(parser != NULL);

// TODO: Add logic for parsing expressions (using LL(1) parsing)
//	return ParserNoMatch();
// }

ParserResult __Parser_parseTypeReference(Parser *parser) {
	// TODO: Add logic to output correct error messages
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	int nullable = false;

	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected type reference in function declaration"),
			Array_fromArgs(1, result.token));
	}

	// Check for '_' identifier
	if(String_equals(result.token->value.string, "_")) {
		return ParserError(
			String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
			Array_fromArgs(1, result.token)
		);
	}


	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);


	// nullable type
	if(peek.token->kind == TOKEN_QUESTION) {
		nullable = true;
		// Skip the '?' token
		LexerResult tmp = Lexer_nextToken(parser->lexer);
		if(!tmp.success) return LexerToParserError(result);

	}

	IdentifierASTNode *paramTypeId = new_IdentifierASTNode(result.token->value.string);
	TypeReferenceASTNode *paramType = new_TypeReferenceASTNode(paramTypeId, nullable);
	return ParserSuccess(paramType);
}

ParserResult __Parser_parseParameter(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	bool isLabeless = false;
	IdentifierASTNode *paramLocalId = NULL;
	IdentifierASTNode *paramExternalId = NULL;
	ExpressionASTNode *initializer = NULL;

	LexerResult peek;
	LexerResult result = Lexer_nextToken(parser->lexer);

	if(!result.success) return LexerToParserError(result);

	// parameter name
	if(result.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected identifier in function declaration"),
			Array_fromArgs(1, result.token));
	}

	// labelless parameter
	if(String_equals(result.token->value.string, "_")) {
		isLabeless = true;
	}

	paramLocalId = new_IdentifierASTNode(result.token->value.string);

	peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	// second parameter name exists
	if(peek.token->type == TOKEN_IDENTIFIER) {
		LexerResult result = Lexer_nextToken(parser->lexer);
		if(!result.success) return LexerToParserError(result);

		// external name is first and local second
		paramExternalId = paramLocalId;
		paramLocalId = new_IdentifierASTNode(result.token->value.string);
	}

	result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->kind != TOKEN_COLON) {
		return ParserError(
			String_fromFormat("expected ':' in function declaration"),
			Array_fromArgs(1, result.token));
	}

	// check for Type
	ParserResult typeResult = __Parser_parseTypeReference(parser);
	if(!typeResult.success) return typeResult;


	// check for initializer
	peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);
	if(peek.token->kind == TOKEN_EQUAL) {
		// Skip the '=' token
		LexerResult tmp = Lexer_nextToken(parser->lexer);
		if(!tmp.success) return LexerToParserError(result);

		ParserResult initializerResult = __Parser_parseExpression(parser);
		if(!initializerResult.success) return initializerResult;
		initializer = (ExpressionASTNode*)initializerResult.node;
	}

	ParameterASTNode *paramNode = new_ParameterASTNode(paramLocalId, (TypeReferenceASTNode*)typeResult.node, initializer, paramExternalId, isLabeless);
	return ParserSuccess(paramNode);
}

ParserResult __Parser_parseParameterList(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->kind != TOKEN_LEFT_PAREN) {
		return ParserError(
			String_fromFormat(
				"expected '(' in argument list of function declaration"),
			Array_fromArgs(1, result.token));
	}

	// parser parameter-list
	Array *parameters = Array_alloc(0);
	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	while(peek.token->kind != TOKEN_RIGHT_PAREN) {

		ParserResult paramResult = __Parser_parseParameter(parser);
		if(!paramResult.success) return paramResult;

		Array_push(parameters, (ParameterASTNode*)paramResult.node);

		peek = Lexer_peekToken(parser->lexer, 1);
		if(!peek.success) return LexerToParserError(peek);

		if(peek.token->kind == TOKEN_COMMA) {
			result = Lexer_nextToken(parser->lexer);
			if(!result.success) return LexerToParserError(result);

		}

	}

	// consume ')'
	result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	ParameterListASTNode *parameterList = new_ParameterListASTNode(parameters);

	return ParserSuccess(parameterList);

}

ParserResult __Parser_parseFuncStatement(Parser *parser) {
	// TODO: Symbol table management
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_FUNC) {
		return ParserError(
			String_fromFormat("expected 'func' in function declaration"),
			Array_fromArgs(1, keyword.token));
	}

	// Consume the identifier
	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	LexerResult peek;


	if(result.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected identifier in function declaration"),
			Array_fromArgs(1, result.token));
	}

	// Check for '_' identifier
	if(String_equals(result.token->value.string, "_")) {
		return ParserError(
			String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
			Array_fromArgs(1, result.token)
		);
	}

	IdentifierASTNode *funcId = new_IdentifierASTNode(result.token->value.string);

	ParserResult parameterListResult = __Parser_parseParameterList(parser);
	if(!parameterListResult.success) return parameterListResult;


	peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	TypeReferenceASTNode *returnType = NULL;
	if(peek.token->kind == TOKEN_ARROW) {
		// Skip the '->' token
		LexerResult tmp = Lexer_nextToken(parser->lexer);
		if(!tmp.success) return LexerToParserError(result);

		ParserResult returnTypeResult = __Parser_parseTypeReference(parser);
		if(!returnTypeResult.success) return returnTypeResult;
		returnType = (TypeReferenceASTNode*)returnTypeResult.node;
	} else {
		// Void return type
		returnType = NULL;
	}


	ParserResult blockResult = __Parser_parseBlock(parser, true);
	if(!blockResult.success) return blockResult;

	FunctionDeclarationASTNode *func = new_FunctionDeclarationASTNode(funcId, (ParameterListASTNode*)parameterListResult.node, returnType, (BlockASTNode*)blockResult.node);
	return ParserSuccess(func);
}

ParserResult __Parser_parsePattern(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected pattern"),
			Array_fromArgs(1, result.token));
	}

	// Check for '_' identifier (has no point, but required by the assignment)
	if(String_equals(result.token->value.string, "_")) {
		return ParserError(
			String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
			Array_fromArgs(1, result.token)
		);
	}

	IdentifierASTNode *patternName = new_IdentifierASTNode(result.token->value.string);
	TypeReferenceASTNode *type = NULL;

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	if(peek.token->kind == TOKEN_COLON) {
		// skip ':'
		LexerResult tmp = Lexer_nextToken(parser->lexer);
		if(!tmp.success) return LexerToParserError(tmp);

		ParserResult typeResult = __Parser_parseTypeReference(parser);
		if(!typeResult.success) return typeResult;
		type = (TypeReferenceASTNode*)typeResult.node;
	}

	PatternASTNode *pattern = new_PatternASTNode(patternName, type);

	return ParserSuccess(pattern);
}

ParserResult __Parser_parseOptionalBindingCondition(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// consume let
	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	if(peek.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("let must be followed by identifier"),
			Array_fromArgs(1, peek.token));
	}

	// Check for '_' identifier (has no point, but required by the assignment)
	if(String_equals(peek.token->value.string, "_")) {
		return ParserError(
			String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
			Array_fromArgs(1, peek.token)
		);
	}

	result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	IdentifierASTNode *id = new_IdentifierASTNode(result.token->value.string);

	OptionalBindingConditionASTNode *bindingCondition = new_OptionalBindingConditionASTNode(id);

	return ParserSuccess(bindingCondition);
}

ParserResult __Parser_parseTest(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	ASTNode *test = NULL;

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	if(peek.token->kind == TOKEN_LEFT_PAREN) {
		LexerResult peek = Lexer_peekToken(parser->lexer, 2);
		if(!peek.success) return LexerToParserError(peek);

		if(peek.token->kind == TOKEN_VAR) {
			return ParserError(
				String_fromFormat("cannot use var in optional binding condition"),
				Array_fromArgs(1, peek.token));
		}

		if(peek.token->kind == TOKEN_LET) {
			return ParserError(
				String_fromFormat("cannot use optional binding in condition with parentheses"),
				Array_fromArgs(1, peek.token));
		}
	}

	if(peek.token->kind == TOKEN_VAR) {
		return ParserError(
			String_fromFormat("cannot use var in optional binding condition"),
			Array_fromArgs(1, peek.token));
	} else if(peek.token->kind == TOKEN_LET) {
		ParserResult bindingConditionResult = __Parser_parseOptionalBindingCondition(parser);
		if(!bindingConditionResult.success) return bindingConditionResult;
		test = (ASTNode*)bindingConditionResult.node;
	} else {
		ParserResult expressionResult = __Parser_parseExpression(parser);
		if(!expressionResult.success) return expressionResult;
		test = (ASTNode*)expressionResult.node;
	}

	return ParserSuccess(test);
}

ParserResult __Parser_parseIfStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_IF) {
		return ParserError(
			String_fromFormat("expected 'if' in if statement"),
			Array_fromArgs(1, keyword.token));
	}

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	// look more into this
	if(peek.token->kind == TOKEN_LEFT_BRACE) {
		return ParserError(
			String_fromFormat("missing condition in 'if' statement"),
			Array_fromArgs(1, peek.token));
	}


	if(peek.token->kind == TOKEN_ELSE || peek.token->type == TOKEN_EOF) {
		return ParserError(
			String_fromFormat("expected expression, var, or let in 'if' condition"),
			Array_fromArgs(1, peek.token));
	}

	ParserResult testResult = __Parser_parseTest(parser);
	if(!testResult.success) return testResult;

	ParserResult blockResult = __Parser_parseBlock(parser, true);
	if(!blockResult.success) return blockResult;

	peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	ASTNode *alternate = NULL;

	if(peek.token->kind == TOKEN_ELSE) {
		// skip else keyword
		LexerResult result = Lexer_nextToken(parser->lexer);
		if(!result.success) return LexerToParserError(result);

		LexerResult peek = Lexer_peekToken(parser->lexer, 1);
		if(!peek.success) return LexerToParserError(peek);

		if(peek.token->kind == TOKEN_IF) {
			ParserResult ifStatementResult = __Parser_parseIfStatement(parser);
			if(!ifStatementResult.success) return ifStatementResult;
			alternate = (ASTNode*)ifStatementResult.node;

		} else {
			ParserResult blockResult = __Parser_parseBlock(parser, true);
			if(!blockResult.success) return blockResult;
			alternate = (ASTNode*)blockResult.node;
		}
	}

	IfStatementASTNode *ifStatement = new_IfStatementASTNode((ASTNode*)testResult.node,  (BlockASTNode*)blockResult.node, (ASTNode*)alternate);

	return ParserSuccess(ifStatement);
}

ParserResult __Parser_parseWhileStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_WHILE) {
		return ParserError(
			String_fromFormat("expected 'while' in while statement"),
			Array_fromArgs(1, keyword.token));
	}

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	// look more into this
	if(peek.token->kind == TOKEN_LEFT_BRACE) {
		return ParserError(
			String_fromFormat("missing condition in 'while' statement"),
			Array_fromArgs(1, peek.token));
	}

	if(peek.token->type == TOKEN_EOF) {
		return ParserError(
			String_fromFormat("expected expression, var, or let in 'while' condition"),
			Array_fromArgs(1, peek.token));
	}

	ParserResult testResult = __Parser_parseTest(parser);
	if(!testResult.success) return testResult;

	ParserResult blockResult = __Parser_parseBlock(parser, true);
	if(!blockResult.success) return blockResult;

	WhileStatementASTNode *whileStatement = new_WhileStatementASTNode((ASTNode*)testResult.node,  (BlockASTNode*)blockResult.node);

	return ParserSuccess(whileStatement);
}

ParserResult __Parser_parseRange(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Parse start expression
	ParserResult startResult = __Parser_parseExpression(parser);
	if(!startResult.success) return startResult;

	// Check for the '...' or '..<' token
	LexerResult operatorResult = Lexer_nextToken(parser->lexer);
	if(!operatorResult.success) return LexerToParserError(operatorResult);

	if(operatorResult.token->kind != TOKEN_RANGE && operatorResult.token->kind != TOKEN_HALF_OPEN_RANGE) {
		return ParserError(
			String_fromFormat("expected '...' or '..<' in range"),
			Array_fromArgs(1, operatorResult.token));
	}

	enum OperatorType operator = operatorResult.token->kind == TOKEN_RANGE ? OPERATOR_RANGE : OPERATOR_HALF_OPEN_RANGE;

	// Parse end expression
	ParserResult endResult = __Parser_parseExpression(parser);
	if(!endResult.success) return endResult;

	// Create the range node
	RangeASTNode *range = new_RangeASTNode((ExpressionASTNode*)startResult.node, (ExpressionASTNode*)endResult.node, operator);

	return ParserSuccess(range);
}

ParserResult __Parser_parseForStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_FOR) {
		return ParserError(
			String_fromFormat("expected 'for' in for statement"),
			Array_fromArgs(1, keyword.token));
	}

	// Parse iterator identifier
	LexerResult iteratorResult = Lexer_nextToken(parser->lexer);
	if(!iteratorResult.success) return LexerToParserError(iteratorResult);

	if(iteratorResult.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected identifier in for statement"),
			Array_fromArgs(1, iteratorResult.token));
	}

	// Check for the 'in' token
	LexerResult inResult = Lexer_nextToken(parser->lexer);
	if(!inResult.success) return LexerToParserError(inResult);

	if(inResult.token->kind != TOKEN_IN) {
		return ParserError(
			String_fromFormat("expected 'in' in for statement"),
			Array_fromArgs(1, inResult.token));
	}

	// Parse range
	ParserResult rangeResult = __Parser_parseRange(parser);
	if(!rangeResult.success) return rangeResult;

	// Parse block
	ParserResult blockResult = __Parser_parseBlock(parser, true);
	if(!blockResult.success) return blockResult;

	// Create the for statement node
	IdentifierASTNode *iterator = new_IdentifierASTNode(iteratorResult.token->value.string);
	ForStatementASTNode *forStatement = new_ForStatementASTNode(iterator, (RangeASTNode*)rangeResult.node, (BlockASTNode*)blockResult.node);

	return ParserSuccess(forStatement);
}

ParserResult __Parser_parseReturnStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_RETURN) {
		return ParserError(
			String_fromFormat("expected 'return' in return statement"),
			Array_fromArgs(1, keyword.token));
	}

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	ExpressionASTNode *expression = NULL;

	if(
		peek.token->type != TOKEN_EOF &&
		peek.token->kind != TOKEN_RIGHT_BRACE &&
		// An empty return followed by another statement
		peek.token->kind != TOKEN_FUNC &&
		peek.token->kind != TOKEN_IF &&
		peek.token->kind != TOKEN_WHILE &&
		peek.token->kind != TOKEN_FOR &&
		peek.token->kind != TOKEN_RETURN &&
		peek.token->kind != TOKEN_BREAK &&
		peek.token->kind != TOKEN_CONTINUE &&
		peek.token->kind != TOKEN_LET &&
		peek.token->kind != TOKEN_VAR
	) {
		ParserResult expressionResult = __Parser_parseExpression(parser);
		if(!expressionResult.success) return expressionResult;
		expression = (ExpressionASTNode*)expressionResult.node;
	}

	ReturnStatementASTNode *returnStatement = new_ReturnStatementASTNode((ExpressionASTNode*)expression);

	return ParserSuccess(returnStatement);
}

ParserResult __Parser_parseBreakStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_BREAK) {
		return ParserError(
			String_fromFormat("expected 'break' in return statement"),
			Array_fromArgs(1, keyword.token));
	}

	BreakStatementASTNode *breakStatement = new_BreakStatementASTNode();

	return ParserSuccess(breakStatement);
}

ParserResult __Parser_parseContinueStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_CONTINUE) {
		return ParserError(
			String_fromFormat("expected 'continue' in return statement"),
			Array_fromArgs(1, keyword.token));
	}

	ContinueStatementASTNode *continueStatement = new_ContinueStatementASTNode();

	return ParserSuccess(continueStatement);
}

ParserResult __Parser_parseVariableDeclarator(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	ParserResult patternResult = __Parser_parsePattern(parser);
	if(!patternResult.success) return patternResult;

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	PatternASTNode *patternNode = (PatternASTNode*)patternResult.node;

	ExpressionASTNode *initializer = NULL;

	if(peek.token->kind == TOKEN_EQUAL) {
		// Consume the `=` token
		LexerResult result = Lexer_nextToken(parser->lexer);
		if(!result.success) return LexerToParserError(result);

		ParserResult initializerResult = __Parser_parseExpression(parser);
		if(!initializerResult.success) return initializerResult;
		initializer = initializerResult.node;
	} else if(!patternNode->type) {
		return ParserError(
			String_fromFormat("type annotation missing in pattern"),
			NULL);
	}

	VariableDeclaratorASTNode *variableDeclarator = new_VariableDeclaratorASTNode(patternNode, (ExpressionASTNode*)initializer);

	return ParserSuccess(variableDeclarator);
}

ParserResult __Parser_parseVariableDeclarationList(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	LexerResult peek;
	LexerResult result;

	Array *declarators = Array_alloc(0);
	peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	while(true) {
		ParserResult declaratorResult = __Parser_parseVariableDeclarator(parser);
		if(!declaratorResult.success) return declaratorResult;

		Array_push(declarators, (VariableDeclaratorASTNode*)declaratorResult.node);

		peek = Lexer_peekToken(parser->lexer, 1);
		if(!peek.success) return LexerToParserError(peek);

		// Consume the `,` token
		if(peek.token->kind == TOKEN_COMMA) {
			result = Lexer_nextToken(parser->lexer);
			if(!result.success) return LexerToParserError(result);
		} else {
			break;
		}
	}

	VariableDeclarationListASTNode *variableDeclarationList = new_VariableDeclarationListASTNode(declarators);

	return ParserSuccess(variableDeclarationList);
}

ParserResult __Parser_parseVariableDeclarationStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Check for the keyword
	LexerResult keyword = Lexer_nextToken(parser->lexer);
	if(!keyword.success) return LexerToParserError(keyword);

	if(keyword.token->kind != TOKEN_VAR && keyword.token->kind != TOKEN_LET) {
		return ParserError(
			String_fromFormat("expected 'var' or 'let' in variable declaration"),
			Array_fromArgs(1, keyword.token));
	}

	bool isConstant = keyword.token->kind == TOKEN_LET;

	ParserResult declarationList = __Parser_parseVariableDeclarationList(parser);
	if(!declarationList.success) return declarationList;

	VariableDeclarationASTNode *variableDeclaration = new_VariableDeclarationASTNode((VariableDeclarationListASTNode*)declarationList.node, isConstant);
	return ParserSuccess(variableDeclaration);
}

ParserResult __Parser_parseArgument(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	IdentifierASTNode *argumentLabel = NULL;
	ExpressionASTNode *expression = NULL;

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	LexerResult peekColon = Lexer_peekToken(parser->lexer, 2);
	if(!peekColon.success) return LexerToParserError(peekColon);


	// labeled argument
	if(peek.token->type == TOKEN_IDENTIFIER && peekColon.token->kind == TOKEN_COLON) {

		LexerResult result = Lexer_nextToken(parser->lexer);
		if(!result.success) return LexerToParserError(result);

		argumentLabel = new_IdentifierASTNode(result.token->value.string);
		// Skip the ':' token
		LexerResult tmp = Lexer_nextToken(parser->lexer);
		if(!tmp.success) return LexerToParserError(result);
	}

	ParserResult expressionResult = __Parser_parseExpression(parser);
	if(!expressionResult.success) return expressionResult;
	expression = (ExpressionASTNode*)expressionResult.node;

	ArgumentASTNode *argument = new_ArgumentASTNode(expression, argumentLabel);

	return ParserSuccess(argument);
}

ParserResult __Parser_parseArgumentList(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Skip the '(' token
	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	// Parse argument-list
	Array *arguments = Array_alloc(0);

	LexerResult peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(peek);

	while(peek.token->kind != TOKEN_RIGHT_PAREN) {
		ParserResult argumentResult = __Parser_parseArgument(parser);
		if(!argumentResult.success) return argumentResult;

		Array_push(arguments, (ArgumentASTNode*)argumentResult.node);

		peek = Lexer_peekToken(parser->lexer, 1);
		if(!peek.success) return LexerToParserError(peek);

		if(peek.token->kind == TOKEN_COMMA) {
			// Skip ','
			result = Lexer_nextToken(parser->lexer);
			if(!result.success) return LexerToParserError(result);

			// Peek to the next argument
			peek = Lexer_peekToken(parser->lexer, 1);
			if(!peek.success) return LexerToParserError(peek);

			// No argument after ','
			if(peek.token->kind == TOKEN_RIGHT_PAREN) {
				return ParserError(
					String_fromFormat("expected expression in argument list"),
					Array_fromArgs(1, peek.token));
			}
		}
	}

	// Skip ')'
	result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	ArgumentListASTNode *argumentList = new_ArgumentListASTNode(arguments);

	return ParserSuccess(argumentList);

}

ParserResult __Parser_parseFunctionCallExpression(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// Get the identifier
	LexerResult identifier = Lexer_nextToken(parser->lexer);
	if(!identifier.success) return LexerToParserError(identifier);

	if(identifier.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected identifier in function call"),
			Array_fromArgs(1, identifier.token));
	}

	// Check for '_' identifier
	if(String_equals(identifier.token->value.string, "_")) {
		return ParserError(
			String_fromFormat("'_' can only appear in a pattern or on the left side of an assignment"),
			Array_fromArgs(1, identifier.token)
		);
	}

	IdentifierASTNode *funcId = new_IdentifierASTNode(identifier.token->value.string);

	ParserResult argumentListResult = __Parser_parseArgumentList(parser);
	if(!argumentListResult.success) return argumentListResult;

	FunctionCallASTNode *fuctionCallExpression = new_FunctionCallASTNode(funcId, (ArgumentListASTNode*)argumentListResult.node);

	return ParserSuccess(fuctionCallExpression);
}

ParserResult __Parser_parseAssignmentStatement(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	// identifier
	LexerResult peek = Lexer_nextToken(parser->lexer);
	if(!peek.success) return LexerToParserError(peek);
	IdentifierASTNode *variableId = new_IdentifierASTNode(peek.token->value.string);

	// skip '='
	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->kind != TOKEN_EQUAL) {
		return ParserError(
			String_fromFormat("expected '=' in assignment statement"),
			Array_fromArgs(1, result.token));
	}

	ParserResult assignmentResult = __Parser_parseExpression(parser);
	if(!assignmentResult.success) return assignmentResult;

	AssignmentStatementASTNode *assignmentStatement = new_AssignmentStatementASTNode(variableId, (ExpressionASTNode*)assignmentResult.node);
	return ParserSuccess(assignmentStatement);
}

ParserResult __Parser_parseStringInterpolation(Parser *parser) {
	assertf(parser != NULL);

	FLUSH_ERROR_BUFFER(parser);

	Array /*<String>*/ *strings = Array_alloc(2);
	Array /*<ExpressionASTNode>*/ *expressions = Array_alloc(1);

	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->kind != TOKEN_STRING) {
		return ParserError(
			String_fromFormat("expected string in string interpolation"),
			Array_fromArgs(1, result.token));
	}

	// Consume the first string
	Array_push(strings, result.token->value.string);

	LexerResult marker = Lexer_peekToken(parser->lexer, 1);
	if(!marker.success) return LexerToParserError(marker);

	while(marker.token->type == TOKEN_STRING_INTERPOLATION_MARKER && marker.token->kind != TOKEN_STRING_TAIL) {
		// Consume the interpolation marker
		LexerResult tmp = Lexer_nextToken(parser->lexer);
		if(!tmp.success) return LexerToParserError(tmp);

		// Consume the expression
		ParserResult expressionResult = __Parser_parseExpression(parser);
		if(!expressionResult.success) return expressionResult;
		Array_push(expressions, expressionResult.node);

		// Consume the marker (this is kinda redundant, but whatever)
		LexerResult markerResult = Lexer_nextToken(parser->lexer);
		if(!markerResult.success) return LexerToParserError(markerResult);
		assertf(markerResult.token->type == TOKEN_STRING_INTERPOLATION_MARKER);

		// Consume the string
		LexerResult stringResult = Lexer_nextToken(parser->lexer);
		if(!stringResult.success) return LexerToParserError(stringResult);
		if(stringResult.token->kind != TOKEN_STRING) {
			return ParserError(
				String_fromFormat("expected string in string interpolation"),
				Array_fromArgs(1, stringResult.token));
		}
		Array_push(strings, stringResult.token->value.string);

		// Peek the next token
		marker = Lexer_peekToken(parser->lexer, 1);
		if(!marker.success) return LexerToParserError(marker);
	}

	InterpolationExpressionASTNode *stringInterpolation = new_InterpolationExpressionASTNode(strings, expressions);
	return ParserSuccess(stringInterpolation);
}

/* How to walk/traverse parsed AST or decide what kind of node the ASTNode
 * pointer refers to in general? */

// void myFunc(ASTNode *node) {
// 	switch(node->type) {
// 		case NODE_PROGRAM: {
// 			ASTNodeProgram *program = (ASTNodeProgram*)node;
// 			myFunc(program->block);
// 		} break;
//
// 		case NODE_BLOCK: {
// 			ASTNodeBlock *block = (ASTNodeBlock*)node;
// 			for(int i = 0; i < block->statements->length; i++) {
// 				myFunc(Array_get(block->statements, i));
// 			}
// 		} break;
//
// 		case NODE_VAR_DECL: {
// 			ASTNodeVarDecl *varDecl = (ASTNodeVarDecl*)node;
// 			myFunc(varDecl->type);
// 		} break;
//
//		...
// 	}
// }

/** End of file src/compiler/parser/Parser.c **/
