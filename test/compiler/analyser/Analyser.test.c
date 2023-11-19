#include <stdio.h>

#include "unit.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/parser/ASTNodes.h"
#include "compiler/analyser/Analyser.h"

#include "../parser/parser_assertions.h"

#define TEST_PRIORITY 70

DESCRIBE(variable_declaraion, "Analysis of variable declaration") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Simple variable declaration") {
		Lexer_setSource(&lexer, "let a = 7");
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Multiple variables declaration") {
		Lexer_setSource(&lexer, "let a = 7, b = 2");
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Multiple types of variable declaration") {
		Lexer_setSource(
			&lexer,
			"let a = 7" LF
			"var b = 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Simple variable declaration with type annotation") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Initialisation of a declared constant variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int" LF
			"a = 7" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Re-initialisation of a declared constant variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int" LF
			"a = 7" LF
			"a = 8" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Re-initialisation of a declared & initialized constant variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
			"a = 8" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Re-declaration of a variable") {
		Lexer_setSource(
			&lexer,
			"let a = 7" LF
			"let a = 3" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Use of invalid type in variable declaration") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
			"let b: Idk = 3" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(variable_resolution, "Resolution of variable references") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Simple variable resolution") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
			"let b: Int = a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Simple variable resolution") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
			"let b: Int = 3 + a * 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();


}
