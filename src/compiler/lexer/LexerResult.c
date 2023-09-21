#include "compiler/lexer/LexerResult.h"

void LexerResult_constructor(LexerResult *result, enum ResultType type, enum Severity severity, String *message, Token *token) {
	Result_constructor((Result*)result, type, severity, message);
	result->token = token;
}

void LexerResult_destructor(LexerResult *result) {
	Result_destructor((Result*)result);
}

LexerResult LexerResult_construct(enum ResultType type, enum Severity severity, String *message,  Token *token) {
	LexerResult result;
	LexerResult_constructor(&result, type, severity, message, token);
	return result;
}
