#include <stdio.h>

#include "unit.h"
#include "parser_assertions.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/parser/ASTNodes.h"


DESCRIBE(variable_declaration, "Variable declaration parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Constant without type annotation") {
		Lexer_setSource(&lexer, "let a = 7");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_VARIABLE_DECLARATION);

		VariableDeclarationASTNode *declaration = (VariableDeclarationASTNode*)statement;
		EXPECT_TRUE(declaration->isConstant);
		EXPECT_NOT_NULL(declaration->declaratorList);

		VariableDeclarationListASTNode *list = declaration->declaratorList;
		EXPECT_NOT_NULL(list->declarators);

		Array *arr = list->declarators;
		EXPECT_EQUAL_INT(arr->size, 1);

		VariableDeclaratorASTNode *declarator = Array_get(arr, 0);
		EXPECT_NOT_NULL(declarator);

		PatternASTNode *pattern = declarator->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NULL(pattern->type);

		IdentifierASTNode *id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "a"));

		LiteralExpressionASTNode *initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type == LITERAL_INTEGER);
		EXPECT_EQUAL_INT(initializer->value.integer, 7);
	} TEST_END();


	TEST_BEGIN("Constant with type annotation") {
	    Lexer_setSource(&lexer, "let hello_string_variable: String = \"hello\"");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_VARIABLE_DECLARATION);

		VariableDeclarationASTNode *declaration = (VariableDeclarationASTNode*)statement;
		EXPECT_TRUE(declaration->isConstant);
		EXPECT_NOT_NULL(declaration->declaratorList);

		VariableDeclarationListASTNode *list = declaration->declaratorList;
		EXPECT_NOT_NULL(list->declarators);

		Array *arr = list->declarators;
		EXPECT_EQUAL_INT(arr->size, 1);

		VariableDeclaratorASTNode *declarator = Array_get(arr, 0);
		EXPECT_NOT_NULL(declarator);

		PatternASTNode *pattern = declarator->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NOT_NULL(pattern->type);
		EXPECT_TRUE(pattern->_type == NODE_PATTERN);
		EXPECT_TRUE(pattern->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(pattern->type->id->name,"String"));

		IdentifierASTNode *id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "hello_string_variable"));

		LiteralExpressionASTNode *initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type == LITERAL_STRING);
		EXPECT_TRUE(String_equals(initializer->value.string, "hello"));

	} TEST_END();

	TEST_BEGIN("More variables declaration") {
	    Lexer_setSource(&lexer, "var a = \"hello\", b: Int = 20, c = 10.12");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_VARIABLE_DECLARATION);

		VariableDeclarationASTNode *declaration = (VariableDeclarationASTNode*)statement;
		EXPECT_FALSE(declaration->isConstant);
		EXPECT_NOT_NULL(declaration->declaratorList);

		VariableDeclarationListASTNode *list = declaration->declaratorList;
		EXPECT_NOT_NULL(list->declarators);

		Array *arr = list->declarators;
		EXPECT_EQUAL_INT(arr->size, 3);

        // first declarator  a = "hello"
		VariableDeclaratorASTNode *declarator = Array_get(arr, 0);
		EXPECT_NOT_NULL(declarator);

		PatternASTNode *pattern = declarator->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NULL(pattern->type);

		IdentifierASTNode *id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "a"));

		LiteralExpressionASTNode *initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type == LITERAL_STRING);
		EXPECT_TRUE(String_equals(initializer->value.string, "hello"));

        // second declarator b = 20
	    declarator = Array_get(arr, 1);
		EXPECT_NOT_NULL(declarator);

		pattern = declarator->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NOT_NULL(pattern->type);
		EXPECT_TRUE(pattern->_type == NODE_PATTERN);
		EXPECT_TRUE(pattern->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(pattern->type->id->name,"Int"));

		id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "b"));

		initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type == LITERAL_INTEGER);
		EXPECT_EQUAL_INT(initializer->value.integer, 20);

        // third declarator c = 10.12
        declarator = Array_get(arr, 2);
		EXPECT_NOT_NULL(declarator);

		pattern = declarator->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NULL(pattern->type);

		id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "c"));

		initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type == LITERAL_FLOATING);
		EXPECT_EQUAL_FLOAT(initializer->value.floating, 10.12);

	} TEST_END();

	TEST_BEGIN("Missing colon") {
        Lexer_setSource(&lexer, "let hello_string_variable String = \"hello\"");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

        EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
        EXPECT_TRUE(result.severity == SEVERITY_ERROR);
        // prbbly later add message check also

	} TEST_END();

}
}
