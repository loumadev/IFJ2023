#include "internal/Array.h"
#include "compiler/lexer/LexerResult.h"

typedef struct Lexer {
	char *source;
	size_t sourceLength;
	char *currentChar;
	Array *tokens;
	int line;
	int column;
	enum WhitespaceType whitespace; // Left whitespace
} Lexer;


void Lexer_constructor(Lexer *lexer);
void Lexer_destructor(Lexer *lexer);
LexerResult Lexer_tokenize(Lexer *lexer, char *source);

void Lexer_printTokens(Lexer *lexer);
