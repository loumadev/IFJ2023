/**
 * @file include/compiler/lexer/LexerResult.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/Result.h"
#include "compiler/lexer/Token.h"

#ifndef LEXER_RESULT_H
#define LEXER_RESULT_H

typedef struct LexerResult {
	bool success;
	enum ResultType type;
	enum Severity severity;
	String *message;
	Array *markers;
	// Extended
	Token *token;
} LexerResult;


void LexerResult_constructor(
	LexerResult *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers,
	Token *token
);
void LexerResult_destructor(LexerResult *result);

LexerResult LexerResult_construct(
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers,
	Token *token
);

#define LexerSuccess() LexerResult_construct(RESULT_SUCCESS, SEVERITY_NONE, NULL, NULL, NULL)
#define LexerNoMatch() LexerResult_construct(RESULT_NO_MATCH, SEVERITY_NONE, NULL, NULL, NULL)
#define LexerError(message, markers) LexerResult_construct(RESULT_ERROR_LEXICAL_ANALYSIS, SEVERITY_ERROR, message, markers, NULL)
#define LexerErrorCustom(type, message, markers) LexerResult_construct(type, SEVERITY_ERROR, message, markers, NULL)

#endif

/** End of file include/compiler/lexer/LexerResult.h **/
