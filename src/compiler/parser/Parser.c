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


/* Definitions of public functions */

void Parser_constructor(Parser *parser) {
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
				String_fromFormat("Expected '{' in block body, but got '%s'", Token_toString(result.token)),
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
				String_fromFormat("Expected '}' in block body, but got '%s'", Token_toString(result.token)),
				Array_fromArgs(1, result.token)
			);
		}
	}

	BlockASTNode *block = new_BlockASTNode(statements);
	return ParserSuccess(block);
}

ParserResult __Parser_parseStatement(Parser *parser) {
	assertf(parser != NULL);

	// TODO: Add logic for parsing statements (using recursive descent)
	return ParserNoMatch();
}

ParserResult __Parser_parseExpression(Parser *parser) {
	assertf(parser != NULL);

	// TODO: Add logic for parsing expressions (using LL(1) parsing)
	return ParserNoMatch();
}

// TODO: Add more functions to parse the rest of the language



/* How to walk/traverse parsed AST or decide what kind of node the ASTNode pointer refers to in general? */

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
