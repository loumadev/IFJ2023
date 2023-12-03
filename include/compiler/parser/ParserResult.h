/**
 * @file include/compiler/parser/ParserResult.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/Result.h"
#include "compiler/parser/ASTNodes.h"

#ifndef PARSER_RESULT_H
#define PARSER_RESULT_H

typedef struct ParserResult {
	bool success;
	enum ResultType type;
	enum Severity severity;
	String *message;
	Array *markers;
	// Extended
	ASTNode *node;
} ParserResult;


void ParserResult_constructor(
	ParserResult *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers,
	ASTNode *node
);
void ParserResult_destructor(ParserResult *result);

ParserResult ParserResult_construct(
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers,
	ASTNode *node
);

#define ParserSuccess(node) ParserResult_construct(RESULT_SUCCESS, SEVERITY_NONE, NULL, NULL, (ASTNode*)node)
#define ParserNoMatch() ParserResult_construct(RESULT_NO_MATCH, SEVERITY_NONE, NULL, NULL, NULL)
#define ParserError(message, markers) ParserResult_construct(RESULT_ERROR_SYNTACTIC_ANALYSIS, SEVERITY_ERROR, message, markers, NULL)
#define LexerToParserError(lexerResult) ParserResult_construct(lexerResult.type, lexerResult.severity, lexerResult.message, lexerResult.markers, NULL)

#endif

/** End of file include/compiler/parser/ParserResult.h **/
