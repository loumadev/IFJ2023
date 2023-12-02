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

	TEST_BEGIN("Type cast of a initializer of constant variable") {
		Lexer_setSource(
			&lexer,
			"let a: Double = 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Type cast of a declared constant variable") {
		Lexer_setSource(
			&lexer,
			"let a: Double" LF
			"a = 2" LF
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

	TEST_BEGIN("Use of uninitialized variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int" LF
			"let b: Int = a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Use of uninitialized nullable variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int?" LF
			"let b: Int? = a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Use of void variable") {
		Lexer_setSource(
			&lexer,
			"let a: Void" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Use of partially initialized variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int" LF
			"if(true) {" LF
			TAB "a = 7" LF
			"}" LF
			"let b: Int = a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Use of partially initialized nullable variable") {
		Lexer_setSource(
			&lexer,
			"let a: Int?" LF
			"if(true) {" LF
			TAB "a = 7" LF
			"}" LF
			"let b: Int? = a" LF
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

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);

		VariableDeclaration *a = Analyser_getVariableByName(&analyser, "a", analyser.globalScope);
		EXPECT_NOT_NULL(a);
		EXPECT_TRUE(a->isInitialized);
		EXPECT_TRUE(a->isUsed);

		VariableDeclaration *b = Analyser_getVariableByName(&analyser, "b", analyser.globalScope);
		EXPECT_NOT_NULL(b);
		EXPECT_TRUE(b->isInitialized);
		EXPECT_FALSE(b->isUsed);

		VariableDeclaratorASTNode *bNode = (VariableDeclaratorASTNode*)b->node;
		EXPECT_NOT_NULL(bNode);

		IdentifierASTNode *aIdentifier = (IdentifierASTNode*)bNode->initializer;
		EXPECT_NOT_NULL(aIdentifier);
		EXPECT_TRUE(aIdentifier->_type == NODE_IDENTIFIER);

		EXPECT_TRUE(aIdentifier->id == a->id);
	} TEST_END();

	TEST_BEGIN("Variable resolution inside the expression") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
			"let b: Int = a * 2" LF
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

		VariableDeclaration *b = Analyser_getVariableByName(&analyser, "b", analyser.globalScope);
		EXPECT_NOT_NULL(b);
		EXPECT_TRUE(b->isInitialized);
		EXPECT_FALSE(b->isUsed);

		VariableDeclaratorASTNode *bNode = (VariableDeclaratorASTNode*)b->node;
		EXPECT_NOT_NULL(bNode);

		EXPECT_BINARY_NODE(bNode->initializer, OPERATOR_MUL, NODE_IDENTIFIER, NODE_LITERAL_EXPRESSION, binary);
		IdentifierASTNode *aIdentifier = (IdentifierASTNode*)binary->left;

		EXPECT_TRUE(aIdentifier->id == a->id);
	} TEST_END();

	TEST_BEGIN("Variable resolution inside the expression") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 7" LF
			"let b: Int = a * 2" LF
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

		VariableDeclaration *b = Analyser_getVariableByName(&analyser, "b", analyser.globalScope);
		EXPECT_NOT_NULL(b);
		EXPECT_TRUE(b->isInitialized);
		EXPECT_FALSE(b->isUsed);

		VariableDeclaratorASTNode *bNode = (VariableDeclaratorASTNode*)b->node;
		EXPECT_NOT_NULL(bNode);

		EXPECT_BINARY_NODE(bNode->initializer, OPERATOR_MUL, NODE_IDENTIFIER, NODE_LITERAL_EXPRESSION, binary);
		IdentifierASTNode *aIdentifier = (IdentifierASTNode*)binary->left;

		EXPECT_TRUE(aIdentifier->id == a->id);
	} TEST_END();

	TEST_BEGIN("Variable resolution inside the function call") {
		Lexer_setSource(
			&lexer,
			"let a = 1" LF
			"write(\"str1\", a, \"str2\")" LF
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

		FunctionCallASTNode *write = (FunctionCallASTNode*)((ExpressionStatementASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT))->expression;
		EXPECT_NOT_NULL(write);
		EXPECT_TRUE(write->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *arg = (IdentifierASTNode*)((ArgumentASTNode*)Array_get(write->argumentList->arguments, 1))->expression;
		EXPECT_NOT_NULL(arg);
		EXPECT_TRUE(arg->_type == NODE_IDENTIFIER);

		EXPECT_NOT_EQUAL_INT(a->id, 0);
		EXPECT_EQUAL_INT(arg->id, a->id);
	} TEST_END();
}

DESCRIBE(type_compatibility_var, "Compatibility of types in variable declaration") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Valid variable initializer") {
		Lexer_setSource(
			&lexer,
			"let a: Int = 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: String = \"Hey!\"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: Bool = false" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		// TODO: Should this be supported?
		// Lexer_setSource(
		// 	&lexer,
		// 	"let a: Double = 2" LF
		// );
		// parserResult = Parser_parse(&parser);
		// EXPECT_TRUE(parserResult.success);

		// analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		// EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid variable initializer") {
		Lexer_setSource(
			&lexer,
			"let a: Int = true" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: String = 3.2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: Int = 3.2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid use of nullable types") {
		Lexer_setSource(
			&lexer,
			"let a: Int? = 12" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: Int? = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: String? = \"Hey!\"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: String? = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid use of nullable types") {
		Lexer_setSource(
			&lexer,
			"let a: Int = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: String = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: Bool = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(type_compatibility_assig, "Compatibility of types in assignemt statement") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Valid variable assignment") {
		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 2" LF
			"a = 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String" LF
			"a = \"Hey!\"" LF
			"a = \"Hello :D\"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Bool" LF
			"a = false" LF
			"a = true" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a == 20) {" LF
			TAB "a = 5" LF
			TAB "var a: String" LF
			TAB "a = \"Hey!\"" LF
			TAB "//var a: String" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid variable assignment") {
		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = true" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String" LF
			"a = 3.2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 3.2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Bool" LF
			"a = \":(\"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid use of nullable types") {
		Lexer_setSource(
			&lexer,
			"var a: Int?" LF
			"a = 12" LF
			"a = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double?" LF
			"a = nil" LF
			"a = 12.8" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String?" LF
			"a = \"Hey!\"" LF
			"a = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Bool?" LF
			"a = false" LF
			"a = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid use of nullable types") {
		Lexer_setSource(
			&lexer,
			"let a: Int" LF
			"a = 5" LF
			"a = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: String" LF
			"a = \"Hey!\"" LF
			"a = nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"let a: Bool" LF
			"a = nil" LF
			"a = false" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(if_statement_analysis, "Analysis of the if statements") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Valid use of if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a == 20) {" LF
			TAB "a = 5" LF
			TAB "var a: String" LF
			TAB "a = \"Hey!\"" LF
			TAB "//var a: String" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a == 20) {" LF
			TAB "a = 5" LF
			TAB "var a: String" LF
			TAB "a = \"Hey!\"" LF
			TAB "//var a: String" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid use of if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a == 20) {" LF
			TAB "a = 5" LF
			TAB "var a: String" LF
			TAB "a = \"Hey!\"" LF
			TAB "var a: String" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a == 20) {" LF
			TAB "a = 5" LF
			TAB "var a: String" LF
			TAB "a = \"Hey!\"" LF
			TAB "a = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid type of test condition in if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a == 6) {" LF
			TAB "a = 5" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String = \"Hey!\"" LF
			"" LF
			"if(a == \"nope\") {" LF
			TAB "let a = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"" LF
			"if(a == b) {" LF
			TAB "let c = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"" LF
			"if(a + b == 8) {" LF // Cast of literal is allowed
			TAB "let d = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = false" LF
			"" LF
			"if((a + b == 5.1) == c) {" LF
			TAB "let d = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = false" LF
			"" LF
			"if(a + b == 0) {" LF
			TAB "let d = 7" LF
			"} else if(a + b != b && (!c || a != 7)) {" LF
			TAB "let d = 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid type of test condition in if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"a = 20" LF
			"" LF
			"if(a) {" LF
			TAB "a = 5" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String = \"Hey!\"" LF
			"" LF
			"if(a) {" LF
			TAB "let a = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"" LF
			"if(a + b) {" LF
			TAB "let c = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = false" LF
			"" LF
			"if(a + b - c) {" LF
			TAB "let d = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = 8" LF
			"" LF
			"if(a + b == c) {" LF // Cast of the variable is not allowed
			TAB "let d = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = false" LF
			"" LF
			"if(a + b == c) {" LF
			TAB "let d = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = false" LF
			"" LF
			"if(a + b == 0) {" LF
			TAB "let d = 7" LF
			"} else if(a + b) {" LF
			TAB "let d = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Double = 3.2" LF
			"let b = 7.6" LF
			"let c = false" LF
			"" LF
			"if(a + b == 0) {" LF
			TAB "let d = 7" LF
			"} else if(a + b == 1) {" LF
			TAB "let d = 7" LF
			TAB "d = 5" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid use of conditional binding in if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 20" LF
			"" LF
			"if let a {" LF
			TAB "var b: Int = a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if let a {" LF
			TAB "var b = a + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if let a {" LF
			TAB "var a = 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if let a {" LF
			TAB "var a = 8 + a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if let a {" LF
			TAB "var b = a + 8" LF
			"} else if(a == 8) {" LF
			TAB "var b = a! + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if(a != 3) {" LF
			TAB "var b = a! + 8" LF
			"} else if let a {" LF
			TAB "var b = a + 8" LF
			"} else {" LF
			TAB "var b = 8" LF
			TAB "b = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid use of conditional binding in if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int = 20" LF
			"" LF
			"if let a {" LF
			TAB "var b: Int = a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);

		Lexer_setSource(
			&lexer,
			"var a: Int = 20" LF
			"" LF
			"if let a {" LF
			TAB "a = 5" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);

		Lexer_setSource(
			&lexer,
			"var a: Int" LF
			"" LF
			"if let a {" LF
			TAB "a = 5" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if let x {" LF
			TAB "var b = a + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int?" LF
			"" LF
			"if let x {" LF
			TAB "var b = a + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if let a {" LF
			TAB "var b = a + 8" LF
			"} else if let x {" LF
			TAB "var b = a! + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if(a == 8) {" LF
			TAB "var b = a + 8" LF
			"} else if let x {" LF
			TAB "var b = a! + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int = nil" LF
			"" LF
			"if(a == 8) {" LF
			TAB "var b = a + 8" LF
			"} else if let a {" LF
			TAB "var b = a! + 8" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int = nil" LF
			"" LF
			"if(a == 8) {" LF
			TAB "var b = a + 8" LF
			"} else if let a {" LF
			TAB "var b = a! + 8" LF
			"} else {" LF
			TAB "var b = 8" LF
			TAB "b = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = nil" LF
			"" LF
			"if(a == 8) {" LF
			TAB "var b = a + 8" LF
			"} else if let x {" LF
			TAB "var b = a! + 8" LF
			"} else {" LF
			TAB "var b = 8" LF
			TAB "b = 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(type_compatibility_expr, "Type compatibility in expressions") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Incompatible types in expression") {

		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String = \"a\"" LF
			"var b: String = \"b\"" LF
			"var c = a + b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Incompatible types in expression") {
		Lexer_setSource(
			&lexer,
			"var a = 1 + true" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = 1.0 + false" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = \"hey\" + false" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = \"hey\" + 0.5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var z = 1" LF
			"var a = \"hey\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Nullably incompatible types in expression") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 1" LF
			"var b = a + 1" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: String? = \"a\"" LF
			"var b: String? = \"b\"" LF
			"var c = a + b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(function_dec_analysis, "Function declaration analysis") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Valid function declaration") {
		{
			Lexer_setSource(
				&lexer,
				"func a() {}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Void {}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int? {return 1}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"" LF
				"a(a: 12)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return x + y}" LF
				"" LF
				"a(a: 1, b: 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int, y b: Int) -> Int {return a + b}" LF
				"" LF
				"a(x: 1, y: 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int, _ b: Int) -> Int {return a + b}" LF
				"" LF
				"a(x: 1, 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int) { }" LF
				"" LF
				"let a = 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"let a = 5" LF
				"" LF
				"func a(x a: Int) { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int) { }" LF
				"" LF
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a + 5" LF
				"}" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a + 5" LF
				"}" LF
				"" LF
				"func a(x a: Int) { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() { }" LF
				"" LF
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a + 5" LF
				"}" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a + 5" LF
				"}" LF
				"" LF
				"func a() { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
	} TEST_END();

	TEST_BEGIN("Invalid function declaration") {
		{
			Lexer_setSource(
				&lexer,
				"func a() {}" LF
				"var b = a + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
			EXPECT_TRUE(analyserResult.type == RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 5" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
			EXPECT_TRUE(analyserResult.type == RESULT_ERROR_SEMANTIC_UNDEFINED_FUNCTION);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int) { }" LF
				"" LF
				"var a = 10" LF
				"var b = a(x: 5)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"" LF
				"var a = 10" LF
				"var b = a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int) { }" LF
				"" LF
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a(x: 5)" LF
				"}" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a(x: 5)" LF
				"}" LF
				"" LF
				"func a(x a: Int) { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() { }" LF
				"" LF
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a()" LF
				"}" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"if(true) {" LF
				TAB "var a = 10" LF
				TAB "var b = a()" LF
				"}" LF
				"" LF
				"func a() { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return false}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return nil}" LF
				"" LF
				"a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a: Int) -> Void {return 1}" LF
				"" LF
				"a(a: 12)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a: Int) {return 1}" LF
				"" LF
				"a(a: 12)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a: Int, b: Bool) -> Int {return a + b}" LF
				"" LF
				"a(a: 1, b: 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Bool {return x + y}" LF
				"" LF
				"a(a: 1, b: 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a a: Int, b b: Int) -> Int {return a + b}" LF
				"" LF
				"a(a: 1, b: 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
			EXPECT_TRUE(analyserResult.type == RESULT_ERROR_SEMANTIC_OTHER);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(x a: Int, y b: Int) -> Int {return x + y}" LF
				"" LF
				"a(x: 1, y: 2)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() { }" LF
				"" LF
				"let a = 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"let a = 5" LF
				"" LF
				"func a() { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, a y: Int) { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b x: Int) { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, a x: Int) { }" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
	} TEST_END();
}

DESCRIBE(function_overloading, "Function overload resolution") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Resolution of non-overloaded function") {
		Lexer_setSource(
			&lexer,
			"func a() {}" LF
			"" LF
			"a()" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);

		ExpressionStatementASTNode *expression = (ExpressionStatementASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
		FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->expression;
		EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

		FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
		EXPECT_NOT_NULL(function);

		FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 0 + FUNCTIONS_COUNT);
		EXPECT_EQUAL_PTR(function->node, declaration);
	} TEST_END();

	TEST_BEGIN("Resolution of non-overloaded function inside an expression") {
		Lexer_setSource(
			&lexer,
			"func a() -> Int {return 1}" LF
			"" LF
			"let v1 = a() + 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);
	} TEST_END();

	TEST_BEGIN("Resolution of non-overloaded function inside an expression") {
		Lexer_setSource(
			&lexer,
			"func a() -> Int {return 1}" LF
			"" LF
			"let v1 = a() + 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);
	} TEST_END();

	TEST_BEGIN("Resolution of overloaded function inside an expression with multiple parameters") {
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(a x: Bool) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(a: false) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(b x: Bool) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(b: false) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(a x: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(a: 1) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return 1}" LF
				"func a(a x: Double, b y: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Int = a(a: 15, b: 8) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 0 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return 1}" LF
				"func a(a x: Double, b y: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(a: 15, b: 8) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return 1}" LF
				"func a(a x: Double, b y: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(a: 15.5, b: 8.5) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return 1}" LF
				"func a(a x: Double, b y: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(a: 15, b: 8.5) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"var v1 = a() > 4" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 0 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
	} TEST_END();


	TEST_BEGIN("Invalid resolution of overloaded function inside an expression with multiple parameters") {
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"let v1: Int = a() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(a x: Bool) -> Double {return 1.5}" LF
				"" LF
				"let v1: Int = a(a: false) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(b x: Bool) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(c: false) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(b x: Bool) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(c: \"hh\") + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int) -> Int {return 1}" LF
				"func a(a x: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Bool = a(a: 1) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return 1}" LF
				"func a(a x: Double, b y: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(a: 15, b: true) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func a(a x: Int, b y: Int) -> Int {return 1}" LF
				"func a(a x: Double, b y: Double) -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a(b: 15, a: 8) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
	} TEST_END();

	TEST_BEGIN("Resolution of overloaded function inside an expression with type hint") {
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}

		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = 5 + a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->right;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}
	} TEST_END();

	TEST_BEGIN("Resolution of overloaded function inside an expression without type hint") {
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"let v1 = a() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 0 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}

		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"let v1 = 5 + a()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 3 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);

			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression->right;
			EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function = Analyser_getFunctionById(&analyser, functionCall->id->id);
			EXPECT_NOT_NULL(function);

			FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)Array_get(statements, 0 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function->node, declaration);
		}

		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"func b() -> Int {return 1}" LF
				"func b() -> Double {return 1.5}" LF
				"" LF
				"let v1 = a() + b() * 6" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
	} TEST_END();

	TEST_BEGIN("Resolution of multiple overloaded functions inside an expression with type hint") {
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"func b() -> Int {return 1}" LF
				"func b() -> Double {return 1.5}" LF
				"" LF
				"let v1: Double = a() + b()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 5 + FUNCTIONS_COUNT);

			VariableDeclaration *variable = Analyser_getVariableByName(&analyser, "v1", analyser.globalScope);
			EXPECT_NOT_NULL(variable);

			BinaryExpressionASTNode *expression = (BinaryExpressionASTNode*)variable->node->initializer;
			EXPECT_TRUE(expression->_type == NODE_BINARY_EXPRESSION);


			FunctionCallASTNode *functionCall1 = (FunctionCallASTNode*)expression->left;
			EXPECT_TRUE(functionCall1->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function1 = Analyser_getFunctionById(&analyser, functionCall1->id->id);
			EXPECT_NOT_NULL(function1);

			FunctionDeclarationASTNode *declaration1 = (FunctionDeclarationASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function1->node, declaration1);


			FunctionCallASTNode *functionCall2 = (FunctionCallASTNode*)expression->right;
			EXPECT_TRUE(functionCall2->_type == NODE_FUNCTION_CALL);

			FunctionDeclaration *function2 = Analyser_getFunctionById(&analyser, functionCall2->id->id);
			EXPECT_NOT_NULL(function2);

			FunctionDeclarationASTNode *declaration2 = (FunctionDeclarationASTNode*)Array_get(statements, 3 + FUNCTIONS_COUNT);
			EXPECT_EQUAL_PTR(function2->node, declaration2);
		}

	} TEST_END();

	TEST_BEGIN("Invalid use of multiple overloaded functions inside an expression without type hint") {
		{
			Lexer_setSource(
				&lexer,
				"func a() -> Int {return 1}" LF
				"func a() -> Double {return 1.5}" LF
				"" LF
				"func b() -> Int {return 1}" LF
				"func b() -> Double {return 1.5}" LF
				"" LF
				"let v1 = a() + b()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}

	} TEST_END();

	TEST_BEGIN("Resolution of overloaded built-in 'write' function") {
		{
			Lexer_setSource(
				&lexer,
				"func write(_ a: Int) -> Int {return 1}" LF
				"" LF
				"let v1 = write(4) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);
		}
		{
			Lexer_setSource(
				&lexer,
				"func write(_ a: Int) -> Int {return 1}" LF
				"" LF
				"write(4.0)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func write(_ a: Int) -> Int {return 1}" LF
				"" LF
				"let v1 = write(4.0) + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func write(_ a: Int) -> Int {return 1}" LF
				"" LF
				"write(true)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);
		}
	} TEST_END();

	TEST_BEGIN("Valid redeclaration of the function overload") {
		{
			Lexer_setSource(
				&lexer,
				"func f() -> Int {return 1}" LF
				"func f() -> Int? {return 1}" LF
				"" LF
				"let v1 = f() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f() {}" LF
				"func f() -> Int? {return 1}" LF
				"" LF
				"let v1 = f()! + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f(a x: Int) {}" LF
				"func f(b x: Int) {}" LF
				"" LF
				"f(a: 5)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		// {
		// 	Lexer_setSource(
		// 		&lexer,
		// 		"func f(a: Int?) {}" LF
		// 		"func f(a: Int) {}" LF
		// 		"" LF
		// 		"f(a: 5)" LF
		// 	);
		// 	parserResult = Parser_parse(&parser);
		// 	EXPECT_TRUE(parserResult.success);

		// 	analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		// 	EXPECT_TRUE(analyserResult.success);
		// }
		{
			Lexer_setSource(
				&lexer,
				"func f(x a: Int) {}" LF
				"func f(y a: Int) {}" LF
				"" LF
				"f(x: 5)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
	} TEST_END();
	TEST_BEGIN("Invalid redeclaration of the function overload") {
		{
			Lexer_setSource(
				&lexer,
				"func f() -> Int {return 1}" LF
				"func f() -> Int {return 1}" LF
				"" LF
				"let v1 = f() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func foo() {}" LF
				"func foo() {}" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f() -> Int? {return 1}" LF
				"func f() -> Int? {return 1}" LF
				"" LF
				"let v1 = f() + 5" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f() {}" LF
				"func f() {}" LF
				"" LF
				"f()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f(a: Int) {}" LF
				"func f(a: Int) {}" LF
				"" LF
				"f(a: 5)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f(a: Int?) {}" LF
				"func f(a: Int?) {}" LF
				"" LF
				"f(a: 5)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f(x a: Int) {}" LF
				"func f(x a: Int) {}" LF
				"" LF
				"f(x: 5)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
	} TEST_END();
}

DESCRIBE(declaration_registry, "Declaration registry") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Registration of global variables in global scope") {
		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 2);
		EXPECT_EQUAL_INT(analyser.functions->size, 0 + FUNCTIONS_COUNT);


		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
			"" LF
			"z = \"hi!\"" LF
			"a = \"hey\\n\" + z" LF
			"" LF
			"var b = \"hey\\n\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 3);
		EXPECT_EQUAL_INT(analyser.functions->size, 0 + FUNCTIONS_COUNT);
	} TEST_END();

	TEST_BEGIN("Registration of global variables in local scope") {
		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
			"" LF
			"if(true) {" LF
			TAB "var z = \"hi!\"" LF
			TAB "var a = \"hey\\n\" + z" LF
			TAB "var b = \"hey\\n\" + z" LF
			"}" LF
			"" LF
			"var b = \"hey\\n\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 6);
		EXPECT_EQUAL_INT(analyser.functions->size, 0 + FUNCTIONS_COUNT);


		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
			"" LF
			"if(true) {" LF
			TAB "var z = \"hi!\"" LF
			TAB "var a = \"hey\\n\" + z" LF
			TAB "if(a == z) {" LF
			TAB TAB "var b = \"hey\\n\" + z" LF
			TAB "}" LF
			TAB "var b = \"hey\\n\" + z" LF
			"}" LF
			"" LF
			"var b = \"hey\\n\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 7);
		EXPECT_EQUAL_INT(analyser.functions->size, 0 + FUNCTIONS_COUNT);
	} TEST_END();

	TEST_BEGIN("Registration of functions in global scope") {
		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
			"" LF
			"func foo() {" LF
			TAB "var z = \"hi!\"" LF
			TAB "var a = \"hey\\n\" + z" LF
			"}" LF
			"" LF
			"var b = \"hey\\n\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 3);
		EXPECT_EQUAL_INT(analyser.functions->size, 1 + FUNCTIONS_COUNT);

		FunctionDeclaration *func1 = Array_get(HashMap_get(analyser.overloads, "foo"), 0);
		EXPECT_NOT_NULL(func1);
		EXPECT_TRUE(String_equals(func1->node->id->name, "foo"));
		EXPECT_EQUAL_PTR(HashMap_get(analyser.functions, String_fromLong(func1->id)->value), func1);
		EXPECT_EQUAL_INT(func1->variables->size, 2);


		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
			"" LF
			"if(true) {" LF
			TAB "var z = \"hi!\"" LF
			TAB "var a = \"hey\\n\" + z" LF
			TAB "if(a == z) {" LF
			TAB TAB "var b = \"hey\\n\" + z" LF
			TAB "}" LF
			TAB "var b = \"hey\\n\" + z" LF
			"}" LF
			"" LF
			"func foo() {" LF
			TAB "var x = \"hi!\"" LF
			TAB "var y = 7" LF
			TAB "var z = false" LF
			"}" LF
			"" LF
			"var b = \"hey\\n\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 7);
		EXPECT_EQUAL_INT(analyser.functions->size, 1 + FUNCTIONS_COUNT);

		FunctionDeclaration *func2 = Array_get(HashMap_get(analyser.overloads, "foo"), 0);
		EXPECT_NOT_NULL(func2);
		EXPECT_TRUE(String_equals(func2->node->id->name, "foo"));
		EXPECT_EQUAL_PTR(HashMap_get(analyser.functions, String_fromLong(func2->id)->value), func2);
		EXPECT_EQUAL_INT(func2->variables->size, 3);


		Lexer_setSource(
			&lexer,
			"var z = \"hello\"" LF
			"var a = \"hey\" + z" LF
			"var q = true" LF
			"" LF
			"if(true) {" LF
			TAB "var z = \"hi!\"" LF
			TAB "var a = \"hey\\n\" + z" LF
			TAB "if(a == z) {" LF
			TAB TAB "var b = \"hey\\n\" + z" LF
			TAB "}" LF
			TAB "var b = \"hey\\n\" + z" LF
			"}" LF
			"" LF
			"func foo() -> Bool {" LF
			TAB "var x = \"hi!\"" LF
			TAB "var y = 7" LF
			TAB "var z = false || q" LF
			TAB "return z" LF
			"}" LF
			"" LF
			"func bar() {" LF
			TAB "var i = 3.2" LF
			TAB "var j = 4.5" LF
			TAB "var k = false" LF
			TAB "if(i == j) {" LF
			TAB TAB "var l = foo() && q || k && j == i" LF
			TAB TAB "while(i < j) {" LF
			TAB TAB TAB "var m = i - j" LF
			TAB TAB TAB "i = i + j * 5e-2" LF
			TAB TAB "}" LF
			TAB TAB "var r = i + j" LF
			TAB "}" LF
			"}" LF
			"" LF
			"var b = \"hey\\n\" + z" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_EQUAL_INT(analyser.variables->size, 8);
		EXPECT_EQUAL_INT(analyser.functions->size, 2 + FUNCTIONS_COUNT);

		FunctionDeclaration *func3 = Array_get(HashMap_get(analyser.overloads, "foo"), 0);
		EXPECT_NOT_NULL(func3);
		EXPECT_TRUE(String_equals(func3->node->id->name, "foo"));
		EXPECT_EQUAL_PTR(HashMap_get(analyser.functions, String_fromLong(func3->id)->value), func3);
		EXPECT_EQUAL_INT(func3->variables->size, 3);

		FunctionDeclaration *func4 = Array_get(HashMap_get(analyser.overloads, "bar"), 0);
		EXPECT_NOT_NULL(func4);
		EXPECT_TRUE(String_equals(func4->node->id->name, "bar"));
		EXPECT_EQUAL_PTR(HashMap_get(analyser.functions, String_fromLong(func4->id)->value), func4);
		EXPECT_EQUAL_INT(func4->variables->size, 6);
	} TEST_END();

	TEST_BEGIN("Resolution of parameters in function declaration") {
		Lexer_setSource(
			&lexer,
			"func foo(_ x: Int, b y: Int) {" LF
			TAB "var n = x + y" LF
			"}" LF
			"" LF
			"foo(5, b: 7)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);

		// Function declaration
		FunctionDeclarationASTNode *funcDeclaration = (FunctionDeclarationASTNode*)Array_get(statements, 0 + FUNCTIONS_COUNT);
		EXPECT_NOT_NULL(funcDeclaration);
		EXPECT_TRUE(funcDeclaration->_type == NODE_FUNCTION_DECLARATION);

		// Function call
		FunctionCallASTNode *functionCall = (FunctionCallASTNode*)((ExpressionStatementASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT))->expression;
		EXPECT_NOT_NULL(functionCall);
		EXPECT_TRUE(functionCall->_type == NODE_FUNCTION_CALL);

		// Parameters
		Array *parameters = funcDeclaration->parameterList->parameters;
		EXPECT_EQUAL_INT(parameters->size, 2);

		ParameterASTNode *parameter1 = Array_get(parameters, 0);
		EXPECT_NOT_NULL(parameter1);
		EXPECT_NOT_EQUAL_INT(parameter1->internalId->id, 0);

		ParameterASTNode *parameter2 = Array_get(parameters, 1);
		EXPECT_NOT_NULL(parameter2);
		EXPECT_NOT_EQUAL_INT(parameter2->internalId->id, 0);

		// Arguments
		Array *arguments = functionCall->argumentList->arguments;
		EXPECT_EQUAL_INT(arguments->size, 2);

		// Variable declaration
		VariableDeclarationASTNode *varDeclaration = (VariableDeclarationASTNode*)Array_get(funcDeclaration->body->statements, 0);
		EXPECT_NOT_NULL(varDeclaration);
		EXPECT_TRUE(varDeclaration->_type == NODE_VARIABLE_DECLARATION);

		VariableDeclaratorASTNode *varDeclarator = (VariableDeclaratorASTNode*)Array_get(varDeclaration->declaratorList->declarators, 0);

		// Binary expression
		EXPECT_BINARY_NODE(varDeclarator->initializer, OPERATOR_PLUS, NODE_IDENTIFIER, NODE_IDENTIFIER, binary);

		IdentifierASTNode *left = (IdentifierASTNode*)binary->left;
		IdentifierASTNode *right = (IdentifierASTNode*)binary->right;

		EXPECT_EQUAL_INT(left->id, parameter1->internalId->id);
		EXPECT_EQUAL_INT(right->id, parameter2->internalId->id);
	} TEST_END();

}

DESCRIBE(return_statement, "Analysis of a return statement") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Valid simple return statement") {
		Lexer_setSource(
			&lexer,
			"func foo() -> Int {" LF
			TAB "return 7" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"func foo() -> String {" LF
			TAB "return \"hey!\"" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"func foo() -> Double {" LF
			TAB "return 7.6" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"func foo() -> Bool {" LF
			TAB "return true" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid simple return statement") {
		Lexer_setSource(
			&lexer,
			"func foo() -> Int {" LF
			TAB "return true" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"func foo() -> String {" LF
			TAB "return 10" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"func foo() -> Double {" LF
			TAB "return \"hh\"" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"func foo() -> Bool {" LF
			TAB "return 1.0" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid return statement with expression") {
		Lexer_setSource(
			&lexer,
			"var a = 7" LF
			"func foo() -> Int {" LF
			TAB "return a * 3 + 2" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = \"hey\"" LF
			"func foo() -> String {" LF
			TAB "return a + \"!\"" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = 7.6" LF
			"func foo() -> Double {" LF
			TAB "return a * 3.2 + 2.1e-2" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = true" LF
			"func foo() -> Bool {" LF
			TAB "return a || false" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid return statement with expression") {
		Lexer_setSource(
			&lexer,
			"var a = 7" LF
			"func foo() -> String {" LF
			TAB "return a * 3 + 2" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = \"hey\"" LF
			"func foo() -> Int {" LF
			TAB "return a + \"!\"" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = 7.6" LF
			"func foo() -> Bool {" LF
			TAB "return a * 3.2 + 2.1e-2" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = true" LF
			"func foo() -> Double {" LF
			TAB "return a || false" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid return statement with nullable expression") {
		Lexer_setSource(
			&lexer,
			"var a = 7" LF
			"func foo() -> Int? {" LF
			TAB "return a * 3 + 2" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = 7" LF
			"func foo() -> Int? {" LF
			TAB "return nil" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid return of the void expression from void function") {
		Lexer_setSource(
			&lexer,
			"func f() {}" LF
			"func g() {return f()}" LF
			"" LF
			"g()" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Valid assignment of the void expression to the void variable") {
		Lexer_setSource(
			&lexer,
			"func f() {}" LF
			"" LF
			"var a: Void = f()" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		// EXPECT_TRUE(analyserResult.success);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid assignment of the void expression to the variable") {
		Lexer_setSource(
			&lexer,
			"func f() {}" LF
			"" LF
			"var a = f()" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid return statement with nullable expression") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int {" LF
			TAB "return a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = 7" LF
			"func foo() -> Int {" LF
			TAB "return nil" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid return statement outside of function") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int {" LF
			TAB "return a" LF
			"}" LF
			"" LF
			"return a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a = 7" LF
			"return nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid return statement with expression inside of function with Void return type") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Void {" LF
			TAB "return a" LF
			"}" LF
			"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);


		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() {" LF
			TAB "return a " LF
			"}" LF
			"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Missing return value inside of function with non-Void return type") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int {" LF
			TAB "return" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(return_reachability, "Return reachability analysis") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Simple return statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "return a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Return statement inside if statemtent") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(true) {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			TAB "return a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Return statement in all branches of the if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(true) {" LF
			TAB TAB "return a" LF
			TAB "} else {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Return statement in all branches of the multibranch if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(true) {" LF
			TAB TAB "return a" LF
			TAB "} else if(false) {" LF
			TAB TAB "return a" LF
			TAB "} else {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Return statement in all branches of the multibranch, multilevel if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(true) {" LF
			TAB TAB "return a" LF
			TAB "} else if(false) {" LF
			TAB TAB "if(a == 2) {" LF
			TAB TAB TAB "return a" LF
			TAB TAB "} else {" LF
			TAB TAB TAB "return a" LF
			TAB TAB "}" LF
			TAB "} else {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Return statement inside while loop") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "while(true) {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			TAB "return a" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Simple missing return statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "var b = a" LF
			"}" LF
			"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Missing return statement inside else branch of if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(false) {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Missing return statement inside one of the branches of if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(false) {" LF
			TAB TAB "return a" LF
			TAB "} else {" LF
			TAB TAB "var b = a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Missing return statement inside missing branch of if statement") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "if(true) {" LF
			TAB TAB "return a" LF
			TAB "} else if(false) {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Missing return statement inside inside statement with returning while loop") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 7" LF
			"func foo() -> Int? {" LF
			TAB "while(true) {" LF
			TAB TAB "return a" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(type_conversion, "Implicit type conversion") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a = 5.0 / 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a = Int2Double(5) / 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a = 1 * (2 * 2) + 8 - 1.5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a = 3 + (2 + (2 * 2) - 6) + (8 - 1.5) - 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a = 3 + (2 * (2 + 2) - 6) + (1 + 8 - 1.5) - 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a = 1 + 1.0 + 1" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a = 1 + 1 + 1.0" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a: Double = 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 1 + FUNCTIONS_COUNT);

		VariableDeclaration *var = Analyser_getVariableByName(&analyser, "a", analyser.globalScope);
		EXPECT_NOT_NULL(var);

		LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)var->node->initializer;
		EXPECT_NOT_NULL(literal);
		EXPECT_TRUE(literal->_type == NODE_LITERAL_EXPRESSION);

		EXPECT_TRUE(literal->type.type == TYPE_DOUBLE);
		EXPECT_EQUAL_FLOAT(literal->value.floating, 5);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a: Double = 5.5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 1 + FUNCTIONS_COUNT);

		VariableDeclaration *var = Analyser_getVariableByName(&analyser, "a", analyser.globalScope);
		EXPECT_NOT_NULL(var);

		LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)var->node->initializer;
		EXPECT_NOT_NULL(literal);
		EXPECT_TRUE(literal->_type == NODE_LITERAL_EXPRESSION);

		EXPECT_TRUE(literal->type.type == TYPE_DOUBLE);
		EXPECT_EQUAL_FLOAT(literal->value.floating, 5.5);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a: Int = 5.5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside a complex expression") {
		Lexer_setSource(
			&lexer,
			"var a = 2.0" LF
			"var b = 2 + (a + 1)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);

		VariableDeclaration *var = Analyser_getVariableByName(&analyser, "b", analyser.globalScope);
		EXPECT_NOT_NULL(var);

		BinaryExpressionASTNode *binary = (BinaryExpressionASTNode*)var->node->initializer;
		EXPECT_NOT_NULL(binary);
		EXPECT_TRUE(binary->_type == NODE_BINARY_EXPRESSION);

		EXPECT_TRUE(binary->type.type == TYPE_DOUBLE);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2" LF
			"var b = a == 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2.0" LF
			"var b = a != 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2" LF
			"var b = a != 5.5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside LT/GT expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2" LF
			"var b = a <= 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2.0" LF
			"var b = a > 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2.0 > 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2" LF
			"var b = a < 5.0" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 2" LF
			"var b = a < 5" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Conversion of literals inside EQ/NEQ expressions") {
		Lexer_setSource(
			&lexer,
			"var a = 2" LF
			"var b = 3.5" LF
			"var c = a >= b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(binary_ops, "Binary operators analysis") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var b = a == nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Int = 5" LF
			"var b = a != nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var b = a > 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Int = 5" LF
			"var b = a > 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Double = 5.5" LF
			"var b = a <= 2.2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Int = 5.0" LF
			"var b = a >= 2" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Int = 5" LF
			"var b = a < 2.0" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(str_interp_analysis, "String interpolation analysis") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Valid use of string interpolation") {
		{
			Lexer_setSource(
				&lexer,
				"var a = \"test\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 5" LF
				"var b = \"pre \\(a) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 5" LF
				"var b = 9" LF
				"var c = \"pre \\(a + b) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a: Double = 9" LF
				"var c = \"pre \\(a + 3) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f() -> Int {return 3}" LF
				"var a = 5" LF
				"var b = 9" LF
				"var c = \"pre \\(1 - (a + b) * f()) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
	} TEST_END();

	TEST_BEGIN("Invalid use of string interpolation") {
		{
			Lexer_setSource(
				&lexer,
				"var a = 5" LF
				"var b = \"pre \\(c) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a: Double = 5" LF
				"var b = 9" LF
				"var c = \"pre \\(a + b) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"func f() -> Bool {return false}" LF
				"var a = 5" LF
				"var b = 9" LF
				"var c = \"pre \\(1 - (a + b) * f()) post\"" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_FALSE(analyserResult.success);
		}
	} TEST_END();
}

DESCRIBE(use_of_builtin_funcs, "Use of built-in functions") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Corrent link between declaration and enum value") {
		Lexer_setSource(
			&lexer,
			"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);

		// readString
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "readString");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_READ_STRING);
		}

		// readInt
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "readInt");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_READ_INT);
		}

		// readDouble
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "readDouble");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_READ_DOUBLE);
		}

		// write
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "write");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_WRITE);
		}

		// Int2Double
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "Int2Double");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_INT_TO_DOUBLE);
		}

		// Double2Int
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "Double2Int");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_DOUBLE_TO_INT);
		}

		// length
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "length");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_LENGTH);
		}

		// substring
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "substring");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_SUBSTRING);
		}

		// ord
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "ord");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_ORD);
		}

		// chr
		{
			Array *declarations = Analyser_getFunctionDeclarationsByName(&analyser, "chr");
			EXPECT_NOT_NULL(declarations);

			FunctionDeclaration *function = Array_get(declarations, 0);
			EXPECT_NOT_NULL(function);

			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_CHR);
		}
	} TEST_END();

	TEST_BEGIN("Valid use of built-in functions") {
		{
			Lexer_setSource(
				&lexer,
				"var a = readInt()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 5" LF
				"var b = Int2Double(a)" LF
				"var c = Double2Int(b)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = readInt()" LF
				"var b = readDouble()" LF
				"var c = Int2Double(a!)" LF
				"var d = c + b!" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = readString()" LF
				"var x = readInt()!" LF
				"var b = length(a!)" LF
				"var c = substring(of: a!, startingAt: 0 + x, endingBefore: b)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 10" LF
				"" LF
				"write(a)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 10" LF
				"var b = 1.5" LF
				"" LF
				"write(a, b)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 10" LF
				"var b = 1.5" LF
				"var c = \"hey\"" LF
				"var d = true" LF
				"var e: Int? = nil" LF
				"" LF
				"write(a, b, c, d, e)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
		{
			Lexer_setSource(
				&lexer,
				"var a = 10" LF
				"var b = 1.5" LF
				"var c = \"hey\"" LF
				"var d = true" LF
				"var e: Int? = nil" LF
				"" LF
				"write(1, \"str\", d || false, b + 4, e, a - 10)" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);
		}
	} TEST_END();

	TEST_BEGIN("Valid use of non-built-in functions") {
		{
			Lexer_setSource(
				&lexer,
				"func foo() -> Void {" LF
				TAB "write(\"Hello, World!\")" LF
				"}" LF
				"" LF
				"foo()" LF
			);
			parserResult = Parser_parse(&parser);
			EXPECT_TRUE(parserResult.success);

			analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
			EXPECT_TRUE(analyserResult.success);

			FunctionDeclaration *function = Array_get(Analyser_getFunctionDeclarationsByName(&analyser, "foo"), 0);
			EXPECT_NOT_NULL(function);
			EXPECT_EQUAL_INT(function->node->builtin, FUNCTION_NONE);

			EXPECT_STATEMENTS(parserResult.node, 2 + FUNCTIONS_COUNT);

			FunctionCallASTNode *call = (FunctionCallASTNode*)((ExpressionStatementASTNode*)Array_get(statements, 1 + FUNCTIONS_COUNT))->expression;
			EXPECT_NOT_NULL(call);
			EXPECT_TRUE(call->_type == NODE_FUNCTION_CALL);

			EXPECT_EQUAL_INT(call->id->id, function->id);
			EXPECT_EQUAL_PTR(function, Analyser_getFunctionById(&analyser, call->id->id));
		}
	} TEST_END();
}

DESCRIBE(for_in_loop, "Analysis of for-in loop") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Simple for-in loop") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("For-in loop with expression as range") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10+6 {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid for-in loop with expression as range 1") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10+6.2 {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid for-in loop with expression as range 2") {
		Lexer_setSource(
			&lexer,
			"for i in 1.5...10+6 {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Invalid for-in loop with expression as range 3") {
		Lexer_setSource(
			&lexer,
			"for i in 1.5...10+6.2 {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("For-in loop with underscore as iterator") {
		Lexer_setSource(
			&lexer,
			"for _ in 1...10+6 {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(break_continue, "Analysis of break/continue statements") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Break outside any loop") {
		Lexer_setSource(
			&lexer,
			"break" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Continue outside any loop") {
		Lexer_setSource(
			&lexer,
			"continue" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Break inside simple loop") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "break" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Continue inside simple loop") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "continue" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Break inside loop with condition") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "if(i == 5) {" LF
			TAB TAB "break" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Continue inside loop with condition") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "if(i == 5) {" LF
			TAB TAB "continue" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Break inside nested loop") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "for j in 1...10 {" LF
			TAB TAB "break" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Continue inside nested loop") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "for j in 1...10 {" LF
			TAB TAB "continue" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Break inside mixed nested loops") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"for i in 1...10 {" LF
			TAB "while a > 0 {" LF
			TAB TAB "break" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Continue inside mixed nested loops") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"while a > 0 {" LF
			TAB "for i in 1...10 {" LF
			TAB TAB "break" LF
			TAB "}" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Redeclaration of loop iterator variable") {
		Lexer_setSource(
			&lexer,
			"for i in 1...10 {" LF
			TAB "var i = 5" LF
			"}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

}

DESCRIBE(ultimate_idk, "Donut.c") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Donut") {
		Lexer_setSource(
			&lexer,
			"" LF
			"let PI = 3.141592" LF
			"let HPI = PI * 0.5" LF
			"" LF
			"func sin(_ x: Double) -> Double {" LF
			"    var result = 0.0" LF
			"    var sign = 1.0" LF
			"    var power = 1.0" LF
			"    while power <= 20.0 {" LF
			"        result = result + (sign * (pow(x, power) / factorial(power)))" LF
			"        sign = 0 - sign" LF
			"        power = power + 2.0" LF
			"    }" LF
			"    return result" LF
			"}" LF
			"" LF
			"func cos(_ x: Double) -> Double {" LF
			"    return sin(HPI - x)" LF
			"}" LF
			"" LF
			"func pow(_ x: Double, _ y: Double) -> Double {" LF
			"    var result = 1.0" LF
			"    var y = y" LF
			"    while y > 0.0 {" LF
			"        result = result * x" LF
			"        y = y - 1.0" LF
			"    }" LF
			"    return result" LF
			"}" LF
			"" LF
			"func factorial(_ n: Double) -> Double {" LF
			"    var result = 1.0" LF
			"    var n = n" LF
			"    while n > 1.0 {" LF
			"        result = result * n" LF
			"        n = n - 1.0" LF
			"    }" LF
			"    return result" LF
			"}" LF
			"" LF
			"" LF
			"var A = 1.0," LF
			"	B = 1.0"LF
			"" LF
			"while(true) {" LF
			"	var b = \"\""LF
			"	var z = \"\""LF
			"	A = A + 0.07"LF
			"	B = B + 0.03"LF
			"	var cA = cos(A),"LF
			"		sA = sin(A),"LF
			"		cB = cos(B),"LF
			"		sB = sin(B)"LF
			"	var m = 0"LF
			"	var k = 0"LF
			"	while(k < 1760) {"LF
			"		if(m == 79) {"LF
			"			b = b + \"\""LF
			"			m = 0"LF
			"		}"LF
			"		else {"LF
			"			b = b + \" \""LF
			"			m = m + 1"LF
			"		}"LF
			"		z = z + \" \""LF
			"" LF
			"		k = k + 1"LF
			"	}"LF
			"	var j = 0.0"LF
			"	while(j < 6.28) {"LF
			"		// j <=> theta"LF
			"		var ct = cos(j),"LF
			"			st = sin(j)"LF
			"		var i = 0.0"LF
			"		while(i < 6.28) {"LF
			"			// i <=> phi"LF
			"			var sp = sin(i),"LF
			"				cp = cos(i),"LF
			"				h = ct + 2, // R1 + R2*cos(theta)"LF
			"				D = 1 / (sp * h * sA + st * cA + 5), // this is 1/z"LF
			"				t = sp * h * cA - st * sA // this is a clever factoring of some of the terms in x' and y'"LF
			"" LF
			"			var x = Double2Int(40 + 30 * D * (cp * h * cB - t * sB)),"LF
			"				y = Double2Int(12 + 15 * D * (cp * h * sB + t * cB)),"LF
			"				o = x + 80 * y,"LF
			"				N ="LF
			"					Double2Int(8 *"LF
			"						((st * sA - sp * ct * cA) * cB -"LF
			"							sp * ct * sA -"LF
			"							st * cA -"LF
			"							cp * ct * sB))"LF
			"			var z_o = Int2Double(ord(substring(of: z, startingAt: o, endingBefore: o + 1)!)) / 255.0"LF
			"			if(y < 22 && y >= 0 && x >= 0 && x < 79 && D > z_o) {"LF
			"				z = substring(of: z, startingAt: 0, endingBefore: o)! + chr(Double2Int(D * 255)) + substring(of: z, startingAt: o + 1, endingBefore: length(z))!"LF
			"				var n = 0"LF
			"				if(N > 0) {"LF
			"					n = N"LF
			"				}"LF
			"				var c = substring(of: \".,-~:;=!*#$@\", startingAt: n, endingBefore: n + 1)!"LF
			"				b = substring(of: b, startingAt: 0, endingBefore: o)! + c + substring(of: b, startingAt: o + 1, endingBefore: length(b))!"LF
			"			}"LF
			"" LF
			"			i = i + 0.02"LF
			"		}"LF
			"" LF
			"		j = j + 0.02"LF
			"	}"LF
			"	"LF
			"	write(b)"LF
			"}" LF
			"" LF
			"" LF
			"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(boolean_ext, "Boolean extension analysis") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Bool? = true" LF
			"var b = (!a)!" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Bool? = true" LF
			"var b = !(a!)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Bool? = true" LF
			"var b = !a!" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"var a: Bool? = true" LF
			"var b = !(a)!" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(havel_tests, "Havel's tests") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Test 19") {
		Lexer_setSource(
			&lexer,
			"var a : Int" LF
			"var b = 3" LF
			"" LF
			"if b == 3 {" LF
			"    a = 4" LF
			"    b = a" LF
			"}" LF
			"else" LF
			"{" LF
			"}" LF
			"" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
		// EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test -1") {
		Lexer_setSource(
			&lexer,
			"let z: Bool? = false" LF
			"let a = !z!" LF
			"write(a)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 57") {
		Lexer_setSource(
			&lexer,
			"let non_nil : Int? = 3" LF
			"let unwrapped = non_nil!" LF
			"write(unwrapped)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 58") {
		Lexer_setSource(
			&lexer,
			"var a = (1 + 2)" LF
			"write(a)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 56") {
		Lexer_setSource(
			&lexer,
			"func foo() { }" LF
			"let a = foo() == foo()" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 65") {
		Lexer_setSource(
			&lexer,
			"if(0 < 1.0) {}" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 69") {
		Lexer_setSource(
			&lexer,
			"if (2 + 1) == 2.0 { }" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 70") {
		Lexer_setSource(
			&lexer,
			"let opt2 : Int? = 5" LF
			"let val2 = (opt2 ?? 1) + 6" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Test 71") {
		Lexer_setSource(
			&lexer,
			"let int = Double2Int(4.4)" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(bin_qe_null, "EQ/NEQ analysis with nullable arguments") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Non-nullable arguments") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"var b = 3" LF
			"" LF
			"var c = a == b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Left argument nullable") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var b = 3" LF
			"" LF
			"var c = a == b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Right argument nullable") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"var b: Int? = 3" LF
			"" LF
			"var c = a == b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Both arguments nullable") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var b: Int? = 3" LF
			"" LF
			"var c = a == b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Left argument direct null") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"" LF
			"var c = nil == a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Right argument direct null") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"" LF
			"var c = a == nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Both arguments direct null") {
		Lexer_setSource(
			&lexer,
			"var c = nil == nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Left argument nullable with right direct null") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var c = a == nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Right argument nullable with left direct null") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var c = nil == a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();
}

DESCRIBE(bin_gt_null, "GT/LT analysis with nullable arguments") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Analyser analyser;
	Analyser_constructor(&analyser);

	ParserResult parserResult;
	AnalyserResult analyserResult;

	TEST_BEGIN("Non-nullable arguments") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"var b = 3" LF
			"" LF
			"var c = a > b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_TRUE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Left argument nullable") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var b = 3" LF
			"" LF
			"var c = a > b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Right argument nullable") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"var b: Int? = 3" LF
			"" LF
			"var c = a > b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Both arguments nullable") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var b: Int? = 3" LF
			"" LF
			"var c = a > b" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Left argument direct null") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"" LF
			"var c = nil > a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Right argument direct null") {
		Lexer_setSource(
			&lexer,
			"var a = 5" LF
			"" LF
			"var c = a > nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Both arguments direct null") {
		Lexer_setSource(
			&lexer,
			"var c = nil > nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Left argument nullable with right direct null") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var c = a > nil" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("Right argument nullable with left direct null") {
		Lexer_setSource(
			&lexer,
			"var a: Int? = 5" LF
			"var c = nil > a" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();

	TEST_BEGIN("IDK") {
		Lexer_setSource(
			&lexer,
			"func f() {}" LF
			"var c = f()" LF
		);
		parserResult = Parser_parse(&parser);
		EXPECT_TRUE(parserResult.success);

		analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
		EXPECT_FALSE(analyserResult.success);
	} TEST_END();
}
