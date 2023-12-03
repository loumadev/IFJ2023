/**
 * @file src/compiler/parser/ParserResult.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/parser/ParserResult.h"

void ParserResult_constructor(
	ParserResult *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers,
	ASTNode *node
) {
	Result_constructor((Result*)result, type, severity, message, markers);

	result->node = node;
}

void ParserResult_destructor(ParserResult *result) {
	Result_destructor((Result*)result);
}

ParserResult ParserResult_construct(
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers,
	ASTNode *node
) {
	ParserResult result;
	ParserResult_constructor(&result, type, severity, message, markers, node);
	return result;
}

/** End of file src/compiler/parser/ParserResult.c **/
