/**
 * @file include/compiler/lexer/Lexer.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
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


/**
 * Constructs an instance of Lexer.
 * @param lexer
 */
void Lexer_constructor(Lexer *lexer);

/**
 * Destructs an instance of Lexer.
 * @param lexer
 */
void Lexer_destructor(Lexer *lexer);

/**
 * Sets the source code to tokenize.
 * @param lexer
 * @param source
 */
void Lexer_setSource(Lexer *lexer, char *source);

/**
 * Returns true if the lexer is at the end of the source code.
 * @param lexer
 * @return true if the lexer is at the end of the source code, false otherwise
 */
bool Lexer_isAtEnd(Lexer *lexer);

/**
 * Returns the next token in token stream.
 * @param lexer
 * @return Next token
 */
LexerResult Lexer_nextToken(Lexer *lexer);

/**
 * Returns the token at the given offset in token stream.
 * @param lexer
 * @param offset
 * @return Token at the given offset
 */
LexerResult Lexer_peekToken(Lexer *lexer, int offset);

/**
 * Tokenizes the whole source code at once and updates the internal token stream.
 * @param lexer
 * @param source
 */
LexerResult Lexer_tokenize(Lexer *lexer, char *source);

void Lexer_printTokens(Lexer *lexer);

#endif

/** End of file include/compiler/lexer/Lexer.h **/
