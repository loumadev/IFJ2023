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
		EXPECT_EQUAL_INT(analyser.functions->size, 0);


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
		EXPECT_EQUAL_INT(analyser.functions->size, 0);
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
		EXPECT_EQUAL_INT(analyser.functions->size, 0);


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
		EXPECT_EQUAL_INT(analyser.functions->size, 0);
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
		EXPECT_EQUAL_INT(analyser.functions->size, 1);

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
		EXPECT_EQUAL_INT(analyser.functions->size, 1);

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
		EXPECT_EQUAL_INT(analyser.functions->size, 2);

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

}

// TODO: Nullable in expression