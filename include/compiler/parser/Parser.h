/**
 * @file include/compiler/parser/Parser.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/parser/ASTNodes.h"
#include "compiler/parser/ParserResult.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/lexer/Token.h"

#ifndef PARSER_H
#define PARSER_H

// TODO: Symbol table management
typedef struct Parser {
	Lexer *lexer;
	LexerResult lastLexerError;
} Parser;

void Parser_constructor(Parser *parser, Lexer *lexer);
void Parser_destructor(Parser *parser);
void Parser_setLexer(Parser *parser, Lexer *lexer);
ParserResult Parser_parse(Parser *parser);

bool Parser_hasLexerError(Parser *parser);
LexerResult Parser_flushLastLexerError(Parser *parser);

#define FLUSH_ERROR_BUFFER(parser) if(Parser_hasLexerError(parser)) return LexerToParserError(Parser_flushLastLexerError(parser))

#endif

/** End of file include/compiler/parser/Parser.h **/
