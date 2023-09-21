#include "internal/Array.h"
#include "compiler/lexer/LexerResult.h"

typedef struct Lexer {
	char *source;
	size_t sourceLength;
	char *currentChar;
	Array *tokens;
	int line;
	int column;
} Lexer;


void Lexer_constructor(Lexer *tokenizer);
void Lexer_destructor(Lexer *tokenizer);
LexerResult Lexer_tokenize(Lexer *tokenizer, char *source);
