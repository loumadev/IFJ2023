#include "internal/Array.h"
#include "compiler/lexer/Result.h"

typedef struct Lexer {
	char *source;
	char *currentChar;
	Array *tokens;
	int line;
	int column;
} Lexer;

void Lexer_constructor(Lexer *tokenizer);
void Lexer_destructor(Lexer *tokenizer);
Result Lexer_tokenize(Lexer *tokenizer, char *source);