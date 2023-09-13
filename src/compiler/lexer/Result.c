#include <stdio.h>
#include <stdbool.h>
#include "compiler/lexer/Result.h"

void Result_constructor(Result *result, enum ResultType type, char *message, Token *token, enum Severity severity) {
	if(!result) return;

	result->type = type;
	result->message = message;
	result->token = token;
	result->severity = severity;

	if(type == RESULT_SUCCESS) {
		result->success = true;
	}
}

void Result_destructor(Result *result) {
	if(!result) return;

	result->type = RESULT_SUCCESS;
	result->message = NULL;
	result->token = NULL;
	result->severity = SEVERITY_NOTE;
	result->success = false;
}

Result __Result_Create(enum ResultType type, char *message, Token *token, enum Severity severity) {
	Result result;
	Result_constructor(&result, type, message, token, severity);
	return result;
}
