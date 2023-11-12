#include <stdio.h>

#include "unit.h"
#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"


DESCRIBE(variable_declaration, "Variable declaration parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Example use of parser") {
		Lexer_setSource(&lexer, "let a = 7");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
	} TEST_END();
}
