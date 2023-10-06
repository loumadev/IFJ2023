#include "compiler/Result.h"
#include "compiler/lexer/Token.h"

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
#define LexerError(message, markers) LexerResult_construct(RESULT_ERROR_STATIC_LEXICAL_ANALYSIS, SEVERITY_ERROR, message, markers, NULL)
