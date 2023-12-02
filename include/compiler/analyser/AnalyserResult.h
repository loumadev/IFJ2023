/**
 * @file include/compiler/analyser/AnalyserResult.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "compiler/Result.h"

#ifndef ANALYSER_RESULT_H
#define ANALYSER_RESULT_H

typedef struct AnalyserResult {
	bool success;
	enum ResultType type;
	enum Severity severity;
	String *message;
	Array *markers;
	// Extended
} AnalyserResult;


void AnalyserResult_constructor(
	AnalyserResult *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers
);
void AnalyserResult_destructor(AnalyserResult *result);

AnalyserResult AnalyserResult_construct(
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers
);

#define AnalyserSuccess() AnalyserResult_construct(RESULT_SUCCESS, SEVERITY_NONE, NULL, NULL)
#define AnalyserNoMatch() AnalyserResult_construct(RESULT_NO_MATCH, SEVERITY_NONE, NULL, NULL)
#define AnalyserError(type, message, markers) AnalyserResult_construct(type, SEVERITY_ERROR, message, markers)

#endif

/** End of file include/compiler/analyser/AnalyserResult.h **/
