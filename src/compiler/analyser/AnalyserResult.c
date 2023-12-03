/**
 * @file src/compiler/analyser/AnalyserResult.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/analyser/AnalyserResult.h"

#include "internal/String.h"
#include "internal/Array.h"
#include "compiler/Result.h"


void AnalyserResult_constructor(
	AnalyserResult *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers
) {
	result->success = (type == RESULT_SUCCESS);
	result->type = type;
	result->severity = severity;
	result->message = message;
	result->markers = markers;
}

void AnalyserResult_destructor(AnalyserResult *result) {
	if(result->message) {
		String_destructor(result->message);
	}
	if(result->markers) {
		Array_destructor(result->markers);
	}
}

AnalyserResult AnalyserResult_construct(
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers
) {
	AnalyserResult result;
	AnalyserResult_constructor(&result, type, severity, message, markers);
	return result;
}

/** End of file src/compiler/analyser/AnalyserResult.c **/
