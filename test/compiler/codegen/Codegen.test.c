#include <stdio.h>

#include "unit.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/analyser/Analyser.h"
#include "compiler/codegen/Codegen.h"
#include "internal/String.h"

#define TEST_PRIORITY 60

DESCRIBE(test_example, "Test example") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	Codegen codegen;
	Codegen_constructor(&codegen, &analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Simple function declaration") {
		Lexer_setSource(&lexer, "func f() {}");
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		Codegen_generate(&codegen);
	} TEST_END();
}