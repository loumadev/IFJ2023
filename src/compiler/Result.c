#include <stdarg.h>
#include "compiler/Result.h"
#include "compiler/lexer/Token.h"

#define RESULT_ERROR_MESSAGE_SIZE 255

void Result_constructor(Result *result, enum ResultType type, enum Severity severity, String *message) {
	result->success = type == RESULT_SUCCESS;
	result->type = type;
	result->severity = severity;
	result->message = message;
	result->markers = Array_alloc(0);
}

void Result_destructor(Result *result) {
	String_destructor(result->message);
}

Result Result_addMarkers(Result *result, ...) {
	va_list args;
	va_start(args, result);

	Token *token = NULL;
	while((token = va_arg(args, Token*))) {
		Array_push(result->markers, token);
	}

	va_end(args);

	return *result;
}