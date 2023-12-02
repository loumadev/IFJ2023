/**
 * @file include/compiler/lexer/Lexer.h
 * @author Author Name <xlogin00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "internal/Array.h"
#include "compiler/lexer/LexerResult.h"

#ifndef LEXER_H
#define LEXER_H

typedef struct Lexer {
	char *source;
	size_t sourceLength;
	char *currentChar;
	Array *tokens;
	int currentTokenIndex;
	int line;
	int column;
	enum WhitespaceType whitespace; // Left whitespace
} Lexer;


void Lexer_constructor(Lexer *lexer);
void Lexer_destructor(Lexer *lexer);
void Lexer_setSource(Lexer *lexer, char *source);
bool Lexer_isAtEnd(Lexer *lexer);
LexerResult Lexer_nextToken(Lexer *lexer);
LexerResult Lexer_peekToken(Lexer *lexer, int offset);
LexerResult Lexer_tokenize(Lexer *lexer, char *source);

void Lexer_printTokens(Lexer *lexer);

#endif

/** End of file include/compiler/lexer/Lexer.h **/
