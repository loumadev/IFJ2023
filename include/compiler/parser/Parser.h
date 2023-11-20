#include "compiler/parser/ASTNodes.h"
#include "compiler/parser/ParserResult.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/lexer/Token.h"

#ifndef PARSER_H
#define PARSER_H

// TODO: Symbol table management
typedef struct Parser {
	Lexer *lexer;
	// SymbolTable *symbolTable;
} Parser;

void Parser_constructor(Parser *parser, Lexer *lexer);
void Parser_destructor(Parser *parser);
void Parser_setLexer(Parser *parser, Lexer *lexer);
ParserResult Parser_parse(Parser *parser);

#endif
