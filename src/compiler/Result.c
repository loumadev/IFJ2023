/**
 * @file src/compiler/Result.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdarg.h>
#include "compiler/Result.h"
#include "compiler/lexer/Token.h"

void Result_constructor(
	Result *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers
) {
	result->success = type == RESULT_SUCCESS || type == RESULT_NO_MATCH;
	result->type = type;
	result->severity = severity;
	result->message = message;
	result->markers = markers;
}

void Result_destructor(Result *result) {
	String_destructor(result->message);
	Array_destructor(result->markers);

	result->success = false;
	result->type = RESULT_INVALID;
	result->severity = SEVERITY_NONE;
	result->message = NULL;
	result->markers = NULL;
}

/** End of file src/compiler/Result.c **/
