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
ParserResult __Parser_parseFuncStatement(Parser *parser);
ParserResult __Parser_parseTypeReference(Parser *parser);
ParserResult __Parser_parseParameter(Parser *parser);
ParserResult __Parser_parseParameterList(Parser *parser);

/* Definitions of public functions */

void Parser_constructor(Parser *parser) {
	// TODO: Symbol table management
	parser->lexer = NULL;
}

void Parser_destructor(Parser *parser) {
	parser->lexer = NULL;
}

void Parser_setLexer(Parser *parser, Lexer *lexer) {
	parser->lexer = lexer;
}

bool Parser_isAtEnd(Parser *parser) {
	assertf(parser != NULL);
	assertf(parser->lexer != NULL);

	return Lexer_isAtEnd(parser->lexer);
}

ParserResult Parser_parse(Parser *parser) {
	assertf(parser != NULL);
	assertf(parser->lexer != NULL);

	return __Parser_parseProgram(parser);
}


/* Definitions of private functions */

ParserResult __Parser_parseProgram(Parser *parser) {
	assertf(parser != NULL);

	ParserResult result = __Parser_parseBlock(parser, false);
	if(!result.success) return result;

	ProgramASTNode *program = new_ProgramASTNode((BlockASTNode*)result.node);
	return ParserSuccess(program);
}

ParserResult __Parser_parseBlock(Parser *parser, bool requireBraces) {
	assertf(parser != NULL);

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

	while(!Parser_isAtEnd(parser)) {
		ParserResult result = __Parser_parseStatement(parser);
		if(!result.success) return result;

		Array_push(statements, result.node);
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

	LexerResult result = Lexer_nextToken(parser->lexer);
	if(!result.success) return LexerToParserError(result);

	if(result.token->kind == TOKEN_FUNC) {
		ParserResult funcResult = __Parser_parseFuncStatement(parser);
		if(!funcResult.success) return funcResult;
	}

	return ParserNoMatch();
}

ParserResult __Parser_parseExpression(Parser *parser) {
	assertf(parser != NULL);

	// TODO: Add logic for parsing expressions (using LL(1) parsing)
	return ParserNoMatch();
}

ParserResult __Parser_parseTypeReference(Parser *parser) {
	// TODO: Add logic to output correct error messages
	assertf(parser != NULL);

	LexerResult result = Lexer_nextToken(parser->lexer);
	LexerResult peek;
	int nullable = false;

	if(!result.success) return LexerToParserError(result);

	if(result.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected type reference in function declaration"),
			Array_fromArgs(1, result.token));
	}

	peek = Lexer_peekToken(parser->lexer, 1);
	if(!peek.success) return LexerToParserError(result);


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
	// TODO: Add logic to output correct error messages
	// TODO: Add expression parsing
	assertf(parser != NULL);

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
		// TODO: Add expression parsing
		//     : Add constructor for ExpressionASTNode
		//     : Expression until , or )
		initializer = NULL;
	}

	ParameterASTNode *paramNode = new_ParameterASTNode(paramLocalId, (TypeReferenceASTNode*)typeResult.node, initializer, paramExternalId, isLabeless);
	return ParserSuccess(paramNode);
}


ParserResult __Parser_parseParameterList(Parser *parser) {
	assertf(parser != NULL);

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
	while(true) {
		ParserResult paramResult = __Parser_parseParameter(parser);
		if(!paramResult.success) return paramResult;

		Array_push(parameters, (ParameterASTNode*)paramResult.node);

		LexerResult peek = Lexer_peekToken(parser->lexer, 1);


		if(!peek.success) return LexerToParserError(result);

		if(peek.token->kind == TOKEN_COMMA) {
			result = Lexer_nextToken(parser->lexer);
			if(!result.success) return LexerToParserError(result);

		}

		peek = Lexer_peekToken(parser->lexer, 1);
		if(peek.token->kind == TOKEN_RIGHT_PAREN) {
			result = Lexer_nextToken(parser->lexer);
			if(!result.success) return LexerToParserError(result);
			break;
		}
	}
	ParameterListASTNode *parameterList = new_ParameterListASTNode(parameters);

	return ParserSuccess(parameterList);

}

ParserResult __Parser_parseFuncStatement(Parser *parser) {
	// TODO: Symbol table management

	assertf(parser != NULL);
	LexerResult result = Lexer_nextToken(parser->lexer);
	LexerResult peek;
	if(!result.success) return LexerToParserError(result);

	if(result.token->type != TOKEN_IDENTIFIER) {
		return ParserError(
			String_fromFormat("expected identifier in function declaration"),
			Array_fromArgs(1, result.token));
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
	// if(!blockResult.success) return blockResult;

	FunctionDeclarationASTNode *func = new_FunctionDeclarationASTNode(funcId, (ParameterListASTNode*)parameterListResult.node, returnType, (BlockASTNode*)blockResult.node);
	return ParserSuccess(func);
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
