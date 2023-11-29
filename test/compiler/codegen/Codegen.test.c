#include <stdio.h>

#include "unit.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/analyser/Analyser.h"
#include "compiler/codegen/Codegen.h"
#include "internal/String.h"

#include "../parser/parser_assertions.h"

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

	// TEST_BEGIN("Initialisation of a declared constant variable") {
	// 	Lexer_setSource(
	// 		&lexer,
	// 		"let a: Int" LF
	// 		"a = 7" LF
	// 	);
	// 	parserResult = Parser_parse(&parser);
	// 	EXPECT_TRUE(parserResult.success);

	// 	analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
	// 	EXPECT_TRUE(analyserResult.success);

	// 	Codegen_generate(&codegen);
	// } TEST_END();

	TEST_BEGIN("Initialisation of a declared constant variable") {
		Lexer_setSource(
			&lexer,
			"let a = 2" LF
			"let b = Int2Double(a)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);

		VariableDeclaration *a = Analyser_getVariableByName(&analyser, "a", analyser.globalScope);
		EXPECT_NOT_NULL(a);
		EXPECT_TRUE(a->isInitialized);
		EXPECT_TRUE(a->isUsed);

		VariableDeclaratorASTNode *b = Array_get(((VariableDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT))->declaratorList->declarators, 0);
		EXPECT_NOT_NULL(b);
		EXPECT_TRUE(b->_type == NODE_VARIABLE_DECLARATOR);

		FunctionCallASTNode *int2double = (FunctionCallASTNode*)b->initializer;
		EXPECT_NOT_NULL(int2double);
		EXPECT_TRUE(int2double->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *arg = (IdentifierASTNode*)((ArgumentASTNode*)Array_get(int2double->argumentList->arguments, 0))->expression;
		EXPECT_NOT_NULL(arg);
		EXPECT_TRUE(arg->_type == NODE_IDENTIFIER);

		EXPECT_NOT_EQUAL_INT(a->id, 0);
		EXPECT_EQUAL_INT(arg->id, a->id);

		Codegen_generate(&codegen);
	} TEST_END();
}