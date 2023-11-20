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

	// TEST_BEGIN("Re-initialisation of a declared constant variable") {
	// 	Lexer_setSource(
	// 		&lexer,
	// 		"let a: Int" LF
	// 		"a = 7" LF
	// 		"a = 8" LF
	// 	);
	// 	parserResult = Parser_parse(&parser);
	// 	EXPECT_TRUE(parserResult.success);

	// 	analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)parserResult.node);
	// 	EXPECT_FALSE(analyserResult.success);
	// } TEST_END();

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

		EXPECT_STATEMENTS(parserResult.node, 2);

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

		EXPECT_STATEMENTS(parserResult.node, 2);

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

		EXPECT_STATEMENTS(parserResult.node, 2);

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

// TODO: Nullable in expression