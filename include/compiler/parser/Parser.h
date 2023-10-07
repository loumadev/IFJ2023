#include "compiler/parser/ASTNodes.h"
#include "compiler/parser/ParserResult.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/lexer/Token.h"

typedef struct Parser {
	Lexer *lexer;
} Parser;

void Parser_constructor(Parser *parser);
void Parser_destructor(Parser *parser);
void Parser_setLexer(Parser *parser, Lexer *lexer);
ParserResult Parser_parse(Parser *parser);
