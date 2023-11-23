#include <stdio.h>

#include "unit.h"
#include "parser_assertions.h"

#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/parser/ASTNodes.h"

#define TEST_PRIORITY 80

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
		EXPECT_TRUE(initializer->type.type == TYPE_INT);
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
		EXPECT_TRUE(String_equals(pattern->type->id->name, "String"));

		IdentifierASTNode *id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "hello_string_variable"));

		LiteralExpressionASTNode *initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type.type == TYPE_STRING);
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
		EXPECT_TRUE(initializer->type.type == TYPE_STRING);
		EXPECT_TRUE(String_equals(initializer->value.string, "hello"));

		// second declarator b = 20
		declarator = Array_get(arr, 1);
		EXPECT_NOT_NULL(declarator);

		pattern = declarator->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NOT_NULL(pattern->type);
		EXPECT_TRUE(pattern->_type == NODE_PATTERN);
		EXPECT_TRUE(pattern->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(pattern->type->id->name, "Int"));

		id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "b"));

		initializer = (LiteralExpressionASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type.type == TYPE_INT);
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
		EXPECT_TRUE(initializer->type.type == TYPE_DOUBLE);
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

	TEST_BEGIN("Missing type and initializer") {
		Lexer_setSource(
			&lexer,
			"let a" LF
			"a = 7" LF
		);
		result = Parser_parse(&parser);
		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);
		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();



}

DESCRIBE(function_declaration, "Function declaration parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("No parameters empty body") {
		Lexer_setSource(&lexer, "func empty_function(){}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_FUNCTION_DECLARATION);

		FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)statement;

		IdentifierASTNode *id = declaration->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "empty_function"));

		// return type
		EXPECT_NULL(declaration->returnType);

		// parameters
		ParameterListASTNode *list = declaration->parameterList;
		EXPECT_NOT_NULL(list->parameters);

		Array *arr = list->parameters;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// body
		BlockASTNode *body = declaration->body;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("With simple parameters empty body") {
		Lexer_setSource(&lexer, "func parameters_function(a: Int, b: Int = 10, c: String = \"hello\"){}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_FUNCTION_DECLARATION);

		FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)statement;

		IdentifierASTNode *id = declaration->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "parameters_function"));

		// return type
		EXPECT_NULL(declaration->returnType);

		// parameters
		ParameterListASTNode *list = declaration->parameterList;
		EXPECT_NOT_NULL(list->parameters);

		Array *arr = list->parameters;
		EXPECT_NOT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 3);

		// first parameter  a: Int
		ParameterASTNode *parameter = Array_get(arr, 0);
		EXPECT_NOT_NULL(parameter);
		EXPECT_FALSE(parameter->isLabeless);

		EXPECT_NULL(parameter->externalId);
		EXPECT_TRUE(String_equals(parameter->internalId->name, "a"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Int"));

		EXPECT_NULL(parameter->initializer);

		// second parameter  b: Int = 10
		parameter = Array_get(arr, 1);
		EXPECT_NOT_NULL(parameter);
		EXPECT_FALSE(parameter->isLabeless);

		EXPECT_NULL(parameter->externalId);
		EXPECT_TRUE(String_equals(parameter->internalId->name, "b"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Int"));

		EXPECT_NOT_NULL(parameter->initializer);

		LiteralExpressionASTNode *initializer = (LiteralExpressionASTNode*)parameter->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(initializer->value.integer, 10);

		// third parameter  c = "hello"
		parameter = Array_get(arr, 2);
		EXPECT_NOT_NULL(parameter);
		EXPECT_FALSE(parameter->isLabeless);

		EXPECT_NULL(parameter->externalId);
		EXPECT_TRUE(String_equals(parameter->internalId->name, "c"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "String"));

		EXPECT_NOT_NULL(parameter->initializer);

		initializer = (LiteralExpressionASTNode*)parameter->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type.type == TYPE_STRING);
		EXPECT_TRUE(String_equals(initializer->value.string, "hello"));

		// body
		BlockASTNode *body = declaration->body;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("With advanced parameters empty body") {
		Lexer_setSource(&lexer, "func parameters_function(a_external a_internal: Int, _ b_internal: Int = 100, _ _: Double = 12.3){}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_FUNCTION_DECLARATION);

		FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)statement;

		IdentifierASTNode *id = declaration->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "parameters_function"));

		// return type
		EXPECT_NULL(declaration->returnType);

		// parameters
		ParameterListASTNode *list = declaration->parameterList;
		EXPECT_NOT_NULL(list->parameters);

		Array *arr = list->parameters;
		EXPECT_NOT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 3);

		// first parameter  a_external a_internal: Int
		ParameterASTNode *parameter = Array_get(arr, 0);
		EXPECT_NOT_NULL(parameter);
		EXPECT_FALSE(parameter->isLabeless);

		EXPECT_TRUE(String_equals(parameter->externalId->name, "a_external"));
		EXPECT_TRUE(String_equals(parameter->internalId->name, "a_internal"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Int"));

		EXPECT_NULL(parameter->initializer);

		// second parameter  _ b_internal: Int = 100
		parameter = Array_get(arr, 1);
		EXPECT_NOT_NULL(parameter);
		EXPECT_TRUE(parameter->isLabeless);

		EXPECT_TRUE(String_equals(parameter->externalId->name, "_"));
		EXPECT_TRUE(String_equals(parameter->internalId->name, "b_internal"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Int"));

		EXPECT_NOT_NULL(parameter->initializer);

		LiteralExpressionASTNode *initializer = (LiteralExpressionASTNode*)parameter->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(initializer->value.integer, 100);

		// third parameter  _ _: Double = 12.3
		parameter = Array_get(arr, 2);
		EXPECT_NOT_NULL(parameter);
		EXPECT_TRUE(parameter->isLabeless);

		EXPECT_TRUE(String_equals(parameter->externalId->name, "_"));
		EXPECT_TRUE(String_equals(parameter->internalId->name, "_"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Double"));

		EXPECT_NOT_NULL(parameter->initializer);

		initializer = (LiteralExpressionASTNode*)parameter->initializer;
		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(initializer->type.type == TYPE_DOUBLE);
		EXPECT_EQUAL_INT(initializer->value.floating, 12.3);

		// body
		BlockASTNode *body = declaration->body;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("With parameters and simple block") {
		Lexer_setSource(&lexer, "func isGreater(_ a:Int, _ b:Int) -> Bool { return a > b}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_FUNCTION_DECLARATION);

		FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)statement;

		IdentifierASTNode *id = declaration->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "isGreater"));

		// return type
		EXPECT_NOT_NULL(declaration->returnType);
		EXPECT_TRUE(declaration->returnType->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(declaration->returnType->id->name, "Bool"));

		// parameters
		ParameterListASTNode *list = declaration->parameterList;
		EXPECT_NOT_NULL(list->parameters);

		Array *arr = list->parameters;
		EXPECT_NOT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 2);

		// first parameter  a: Int
		ParameterASTNode *parameter = Array_get(arr, 0);
		EXPECT_NOT_NULL(parameter);
		EXPECT_TRUE(parameter->isLabeless);

		EXPECT_TRUE(String_equals(parameter->externalId->name, "_"));
		EXPECT_TRUE(String_equals(parameter->internalId->name, "a"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Int"));

		EXPECT_NULL(parameter->initializer);

		// second parameter  a: Int
		parameter = Array_get(arr, 1);
		EXPECT_NOT_NULL(parameter);
		EXPECT_TRUE(parameter->isLabeless);

		EXPECT_TRUE(String_equals(parameter->externalId->name, "_"));
		EXPECT_TRUE(String_equals(parameter->internalId->name, "b"));

		EXPECT_NOT_NULL(parameter->type);
		EXPECT_TRUE(parameter->type->_type == NODE_TYPE_REFERENCE);
		EXPECT_TRUE(String_equals(parameter->type->id->name, "Int"));

		EXPECT_NULL(parameter->initializer);

		// body
		BlockASTNode *body = declaration->body;
		EXPECT_NOT_NULL(body->statements);

		arr = body->statements;
		EXPECT_NOT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 1);

		StatementASTNode *body_statement = Array_get(arr, 0);
		EXPECT_TRUE(body_statement->_type == NODE_RETURN_STATEMENT);
		ReturnStatementASTNode *return_statement = (ReturnStatementASTNode*)body_statement;

		BinaryExpressionASTNode *function_return = (BinaryExpressionASTNode*)return_statement->expression;
		EXPECT_NOT_NULL(function_return);
		EXPECT_TRUE(function_return->_type == NODE_BINARY_EXPRESSION);

		EXPECT_TRUE(function_return->left);

		IdentifierASTNode *left = (IdentifierASTNode*)function_return->left;
		EXPECT_NOT_NULL(left);
		EXPECT_TRUE(left->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(left->name, "a"));

		IdentifierASTNode *right = (IdentifierASTNode*)function_return->right;
		EXPECT_NOT_NULL(right);
		EXPECT_TRUE(right->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(right->name, "b"));

		EXPECT_TRUE(function_return->operator == OPERATOR_GREATER);

	} TEST_END();

	TEST_BEGIN("With return null") {
		Lexer_setSource(
			&lexer,
			"func foo() {" LF
			TAB "return" LF
			"}" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);


		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_FUNCTION_DECLARATION);

		FunctionDeclarationASTNode *declaration = (FunctionDeclarationASTNode*)statement;

		IdentifierASTNode *id = declaration->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		// return type
		EXPECT_NULL(declaration->returnType);

		// parameters
		ParameterListASTNode *list = declaration->parameterList;
		EXPECT_NOT_NULL(list->parameters);

		Array *arr = list->parameters;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// body
		BlockASTNode *body = declaration->body;
		EXPECT_NOT_NULL(body->statements);

		arr = body->statements;
		EXPECT_NOT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 1);

		StatementASTNode *body_statement = Array_get(arr, 0);
		EXPECT_TRUE(body_statement->_type == NODE_RETURN_STATEMENT);
		ReturnStatementASTNode *return_statement = (ReturnStatementASTNode*)body_statement;
		EXPECT_NULL(return_statement->expression);

	} TEST_END();

}

DESCRIBE(if_statement, "If statement parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Simple condition no body, no else") {
		Lexer_setSource(&lexer, "if (true) {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_IF_STATEMENT);

		IfStatementASTNode *if_statement = (IfStatementASTNode*)statement;

		EXPECT_NOT_NULL(if_statement->test);
		EXPECT_TRUE(if_statement->test->_type == NODE_LITERAL_EXPRESSION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)if_statement->test;
		EXPECT_TRUE(condition_expression->type.type == TYPE_BOOL);
		EXPECT_TRUE(condition_expression->value.boolean);

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else
		EXPECT_NULL(if_statement->alternate);

	} TEST_END();

	TEST_BEGIN("Simple condition no parens, no body") {
		Lexer_setSource(&lexer, "if true {} else {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_IF_STATEMENT);

		IfStatementASTNode *if_statement = (IfStatementASTNode*)statement;

		EXPECT_NOT_NULL(if_statement->test);
		EXPECT_TRUE(if_statement->test->_type == NODE_LITERAL_EXPRESSION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)if_statement->test;
		EXPECT_TRUE(condition_expression->type.type == TYPE_BOOL);
		EXPECT_TRUE(condition_expression->value.boolean);

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else body
		EXPECT_TRUE(if_statement->alternate->_type == NODE_BLOCK);
		body = (BlockASTNode*)if_statement->alternate;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("Else if condition, no body") {
		Lexer_setSource(&lexer, "if a > 10 {}  else if (a < 10) {} else {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_IF_STATEMENT);

		// if
		IfStatementASTNode *if_statement = (IfStatementASTNode*)statement;

		EXPECT_NOT_NULL(if_statement->test);
		EXPECT_FALSE(if_statement->test->_type == NODE_EXPRESSION_STATEMENT);

		BinaryExpressionASTNode *condition_expression = (BinaryExpressionASTNode*)if_statement->test;

		IdentifierASTNode *left = (IdentifierASTNode*)condition_expression->left;
		EXPECT_NOT_NULL(left);
		EXPECT_TRUE(left->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(left->name, "a"));

		LiteralExpressionASTNode *right = (LiteralExpressionASTNode*)condition_expression->right;
		EXPECT_NOT_NULL(right);
		EXPECT_TRUE(right->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(right->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(right->value.integer, 10);

		EXPECT_TRUE(condition_expression->operator == OPERATOR_GREATER);

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else if
		EXPECT_NOT_NULL(if_statement->alternate);

		EXPECT_TRUE(if_statement->alternate->_type == NODE_IF_STATEMENT);

		IfStatementASTNode *elseif = (IfStatementASTNode*)if_statement->alternate;

		EXPECT_NOT_NULL(elseif->test);
		EXPECT_TRUE(elseif->test->_type == NODE_BINARY_EXPRESSION);

		condition_expression = (BinaryExpressionASTNode*)elseif->test;

		left = (IdentifierASTNode*)condition_expression->left;
		EXPECT_NOT_NULL(left);
		EXPECT_TRUE(left->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(left->name, "a"));

		right = (LiteralExpressionASTNode*)condition_expression->right;
		EXPECT_NOT_NULL(right);
		EXPECT_TRUE(right->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(right->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(right->value.integer, 10);

		EXPECT_TRUE(condition_expression->operator == OPERATOR_LESS);

		// else if body
		body = elseif->body;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else body
		EXPECT_TRUE(elseif->alternate->_type == NODE_BLOCK);
		body = (BlockASTNode*)elseif->alternate;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("Binding condition with parantheses") {
		Lexer_setSource(&lexer, "if (let b) {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition with initializer") {
		Lexer_setSource(&lexer, "if let hello = world {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition with type and initializer") {
		Lexer_setSource(&lexer, "if let hello: Int = 10 {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition with var") {
		Lexer_setSource(&lexer, "if var hello = world {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition no body, no else") {
		Lexer_setSource(&lexer, "if let b {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_IF_STATEMENT);

		IfStatementASTNode *if_statement = (IfStatementASTNode*)statement;

		EXPECT_NOT_NULL(if_statement->test);
		EXPECT_TRUE(if_statement->test->_type == NODE_OPTIONAL_BINDING_CONDITION);

		OptionalBindingConditionASTNode *binding_condition = (OptionalBindingConditionASTNode*)if_statement->test;

		IdentifierASTNode *id = binding_condition->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "b"));

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else
		EXPECT_NULL(if_statement->alternate);

	} TEST_END();
}

DESCRIBE(while_statement, "While statement parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Simple condition no body") {
		Lexer_setSource(&lexer, "while (true) {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_WHILE_STATEMENT);

		WhileStatementASTNode *while_statement = (WhileStatementASTNode*)statement;

		EXPECT_NOT_NULL(while_statement->test);
		EXPECT_TRUE(while_statement->test->_type == NODE_LITERAL_EXPRESSION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)while_statement->test;
		EXPECT_TRUE(condition_expression->type.type == TYPE_BOOL);
		EXPECT_TRUE(condition_expression->value.boolean);

		// while body
		BlockASTNode *body = while_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("Simple condition no parens, no body") {
		Lexer_setSource(&lexer, "while true {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_WHILE_STATEMENT);

		WhileStatementASTNode *while_statement = (WhileStatementASTNode*)statement;

		EXPECT_NOT_NULL(while_statement->test);
		EXPECT_TRUE(while_statement->test->_type == NODE_LITERAL_EXPRESSION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)while_statement->test;
		EXPECT_TRUE(condition_expression->type.type == TYPE_BOOL);
		EXPECT_TRUE(condition_expression->value.boolean);

		// while body
		BlockASTNode *body = while_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();

	TEST_BEGIN("Binding condition with parantheses") {
		Lexer_setSource(&lexer, "while (let hello) {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition with initializer") {
		Lexer_setSource(&lexer, "while let hello = world {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition with type and initializer") {
		Lexer_setSource(&lexer, "while let hello: Int = 10 {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition with var") {
		Lexer_setSource(&lexer, "while var hello = world {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition no body") {
		Lexer_setSource(&lexer, "while let hello {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_WHILE_STATEMENT);

		WhileStatementASTNode *while_statement = (WhileStatementASTNode*)statement;

		EXPECT_NOT_NULL(while_statement->test);
		EXPECT_TRUE(while_statement->test->_type == NODE_OPTIONAL_BINDING_CONDITION);

		OptionalBindingConditionASTNode *binding_condition = (OptionalBindingConditionASTNode*)while_statement->test;

		IdentifierASTNode *id = binding_condition->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "hello"));

		// while body
		BlockASTNode *body = while_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

	} TEST_END();
}

DESCRIBE(statement_separation, "Validity of statement separation") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Single statement") {
		Lexer_setSource(&lexer, "var a = 10");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_VARIABLE_DECLARATION);
	} TEST_END();

	TEST_BEGIN("Multiple statements on new lines") {
		Lexer_setSource(
			&lexer,
			"var a = 10" LF
			"var b = 20" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENTS(result.node, 2);
	} TEST_END();

	TEST_BEGIN("Multiple statements on new lines") {
		Lexer_setSource(
			&lexer,
			"func a() {return}" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENTS(result.node, 1);
	} TEST_END();

	TEST_BEGIN("Multiple statements on new lines") {
		Lexer_setSource(
			&lexer,
			"func a() {" LF
			TAB "var a = 10" LF
			TAB "return}" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENTS(result.node, 1);
	} TEST_END();

	TEST_BEGIN("Multiple statements on same line") {
		Lexer_setSource(
			&lexer,
			"var a = 10 var b = 20" LF
		);
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
	} TEST_END();
}

DESCRIBE(function_calls, "Function call parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Simple function call") {
		Lexer_setSource(
			&lexer,
			"a = foo()" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));
	} TEST_END();

	TEST_BEGIN("Simple function call in context") {
		Lexer_setSource(
			&lexer,
			"a = foo()" LF
			"b = 7" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENTS(result.node, 2);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)Array_get(statements, 0);

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));
	} TEST_END();

	TEST_BEGIN("Function call with a single parameter") {
		Lexer_setSource(
			&lexer,
			"a = foo(1)" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 1);


		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument);

		IdentifierASTNode *label = argument->label;
		EXPECT_NULL(label);

		LiteralExpressionASTNode *expression = (LiteralExpressionASTNode*)argument->expression;
		EXPECT_NOT_NULL(expression);
		EXPECT_TRUE(expression->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(expression->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(expression->value.integer, 1);
	} TEST_END();

	TEST_BEGIN("Function call with a single labeled parameter") {
		Lexer_setSource(
			&lexer,
			"a = foo(bar: 1)" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 1);


		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument);

		IdentifierASTNode *label = argument->label;
		EXPECT_NOT_NULL(label);
		EXPECT_TRUE(String_equals(label->name, "bar"));

		LiteralExpressionASTNode *expression = (LiteralExpressionASTNode*)argument->expression;
		EXPECT_NOT_NULL(expression);
		EXPECT_TRUE(expression->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(expression->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(expression->value.integer, 1);
	} TEST_END();

	TEST_BEGIN("Function call with multiple parameters") {
		Lexer_setSource(
			&lexer,
			"a = foo(1, 7)" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 2);


		ArgumentASTNode *argument1 = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument1);

		IdentifierASTNode *label1 = argument1->label;
		EXPECT_NULL(label1);

		LiteralExpressionASTNode *expression1 = (LiteralExpressionASTNode*)argument1->expression;
		EXPECT_NOT_NULL(expression1);
		EXPECT_TRUE(expression1->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(expression1->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(expression1->value.integer, 1);


		ArgumentASTNode *argument2 = (ArgumentASTNode*)Array_get(arr, 1);
		EXPECT_NOT_NULL(argument2);

		IdentifierASTNode *label2 = argument2->label;
		EXPECT_NULL(label2);

		LiteralExpressionASTNode *expression2 = (LiteralExpressionASTNode*)argument2->expression;
		EXPECT_NOT_NULL(expression2);
		EXPECT_TRUE(expression2->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(expression2->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(expression2->value.integer, 7);
	} TEST_END();

	TEST_BEGIN("Function call with multiple labeled parameters") {
		Lexer_setSource(
			&lexer,
			"a = foo(bar: 1, baz: 7)" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 2);


		ArgumentASTNode *argument1 = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument1);

		IdentifierASTNode *label1 = argument1->label;
		EXPECT_NOT_NULL(label1);
		EXPECT_TRUE(String_equals(label1->name, "bar"));

		LiteralExpressionASTNode *expression1 = (LiteralExpressionASTNode*)argument1->expression;
		EXPECT_NOT_NULL(expression1);
		EXPECT_TRUE(expression1->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(expression1->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(expression1->value.integer, 1);


		ArgumentASTNode *argument2 = (ArgumentASTNode*)Array_get(arr, 1);
		EXPECT_NOT_NULL(argument2);

		IdentifierASTNode *label2 = argument2->label;
		EXPECT_NOT_NULL(label2);
		EXPECT_TRUE(String_equals(label2->name, "baz"));

		LiteralExpressionASTNode *expression2 = (LiteralExpressionASTNode*)argument2->expression;
		EXPECT_NOT_NULL(expression2);
		EXPECT_TRUE(expression2->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(expression2->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(expression2->value.integer, 7);
	} TEST_END();

	TEST_BEGIN("Function call in simple expression") {
		Lexer_setSource(
			&lexer,
			"a = foo() * 2" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		EXPECT_BINARY_NODE(assignment->expression, OPERATOR_MUL, NODE_FUNCTION_CALL, NODE_LITERAL_EXPRESSION, binary);


		FunctionCallASTNode *function_call = (FunctionCallASTNode*)binary->left;
		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 0);


		LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->right;
		EXPECT_TRUE(literal->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(literal->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(literal->value.integer, 2);
	} TEST_END();

	TEST_BEGIN("Function call with arguments in simple expression") {
		Lexer_setSource(
			&lexer,
			"a = foo(4) * 2" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		EXPECT_BINARY_NODE(assignment->expression, OPERATOR_MUL, NODE_FUNCTION_CALL, NODE_LITERAL_EXPRESSION, binary);


		FunctionCallASTNode *function_call = (FunctionCallASTNode*)binary->left;
		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 1);

		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument);
		EXPECT_NULL(argument->label);

		LiteralExpressionASTNode *argLiteral = (LiteralExpressionASTNode*)argument->expression;
		EXPECT_NOT_NULL(argLiteral);
		EXPECT_TRUE(argLiteral->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(argLiteral->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(argLiteral->value.integer, 4);


		LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->right;
		EXPECT_TRUE(literal->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(literal->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(literal->value.integer, 2);
	} TEST_END();

	TEST_BEGIN("Function call in complex expression") {
		Lexer_setSource(
			&lexer,
			"a = 4 + (8 - foo(7)) * 2" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		EXPECT_BINARY_NODE(assignment->expression, OPERATOR_PLUS, NODE_LITERAL_EXPRESSION, NODE_BINARY_EXPRESSION, binary1);

		EXPECT_BINARY_NODE(binary1->right, OPERATOR_MUL, NODE_BINARY_EXPRESSION, NODE_LITERAL_EXPRESSION, binary2);

		EXPECT_BINARY_NODE(binary2->left, OPERATOR_MINUS, NODE_LITERAL_EXPRESSION, NODE_FUNCTION_CALL, binary3);

		// TODO: Add more checks here
	} TEST_END();

	TEST_BEGIN("Expression inside function call") {
		Lexer_setSource(
			&lexer,
			"a = foo(3 * 8)" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 1);

		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument);
		EXPECT_NULL(argument->label);

		EXPECT_BINARY_NODE(argument->expression, OPERATOR_MUL, NODE_LITERAL_EXPRESSION, NODE_LITERAL_EXPRESSION, binary);
	} TEST_END();

	TEST_BEGIN("@Fothsid's function call") {
		Lexer_setSource(
			&lexer,
			"a = foo(bar(100/bar(10))+50)" LF
		);
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)assignment->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		Array *arr = function_call->argumentList->arguments;
		EXPECT_EQUAL_INT(arr->size, 1);

		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(argument);
		EXPECT_NULL(argument->label);


		EXPECT_BINARY_NODE(argument->expression, OPERATOR_PLUS, NODE_FUNCTION_CALL, NODE_LITERAL_EXPRESSION, binaryAdd);

		FunctionCallASTNode *function_call2 = (FunctionCallASTNode*)binaryAdd->left;
		EXPECT_NOT_NULL(function_call2);
		EXPECT_TRUE(function_call2->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id2 = function_call2->id;
		EXPECT_NOT_NULL(id2);
		EXPECT_TRUE(String_equals(id2->name, "bar"));

		Array *arr2 = function_call2->argumentList->arguments;
		EXPECT_EQUAL_INT(arr2->size, 1);

		ArgumentASTNode *argument2 = (ArgumentASTNode*)Array_get(arr2, 0);
		EXPECT_NOT_NULL(argument2);
		EXPECT_NULL(argument2->label);


		EXPECT_BINARY_NODE(argument2->expression, OPERATOR_DIV, NODE_LITERAL_EXPRESSION, NODE_FUNCTION_CALL, binaryDiv);

		LiteralExpressionASTNode *literal1 = (LiteralExpressionASTNode*)binaryDiv->left;
		EXPECT_NOT_NULL(literal1);
		EXPECT_TRUE(literal1->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(literal1->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(literal1->value.integer, 100);

		FunctionCallASTNode *function_call3 = (FunctionCallASTNode*)binaryDiv->right;
		EXPECT_NOT_NULL(function_call3);
		EXPECT_TRUE(function_call3->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id3 = function_call3->id;
		EXPECT_NOT_NULL(id3);
		EXPECT_TRUE(String_equals(id3->name, "bar"));

		Array *arr3 = function_call3->argumentList->arguments;
		EXPECT_EQUAL_INT(arr3->size, 1);

		ArgumentASTNode *argument3 = (ArgumentASTNode*)Array_get(arr3, 0);
		EXPECT_NOT_NULL(argument3);
		EXPECT_NULL(argument3->label);

		LiteralExpressionASTNode *literal2 = (LiteralExpressionASTNode*)argument3->expression;
		EXPECT_NOT_NULL(literal2);
		EXPECT_TRUE(literal2->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(literal2->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(literal2->value.integer, 10);

		LiteralExpressionASTNode *literal3 = (LiteralExpressionASTNode*)binaryAdd->right;
		EXPECT_NOT_NULL(literal3);
		EXPECT_TRUE(literal3->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(literal3->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(literal3->value.integer, 50);
	} TEST_END();

	TEST_BEGIN("Statement function call no arguments") {
		Lexer_setSource(
			&lexer,
			"foo()"
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_EXPRESSION_STATEMENT);

		ExpressionStatementASTNode *expression_statement = (ExpressionStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)expression_statement->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		EXPECT_NOT_NULL(function_call->argumentList)
		Array *arguments = (Array*)function_call->argumentList->arguments;

		EXPECT_NULL(arguments->data);
		EXPECT_TRUE(arguments->size == 0);


	} TEST_END();

	TEST_BEGIN("Statement function call with function call argument") {
		Lexer_setSource(
			&lexer,
			"foo(bar(1), b:10)"
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_EXPRESSION_STATEMENT);

		ExpressionStatementASTNode *expression_statement = (ExpressionStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)expression_statement->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "foo"));

		EXPECT_NOT_NULL(function_call->argumentList)
		Array *arguments = (Array*)function_call->argumentList->arguments;

		EXPECT_NOT_NULL(arguments->data);
		EXPECT_TRUE(arguments->size == 2);

		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arguments, 0);
		EXPECT_NOT_NULL(argument);
		EXPECT_NULL(argument->label);

		// first argument bar(1)
		FunctionCallASTNode *first_argument_expression = (FunctionCallASTNode*)argument->expression;
		EXPECT_NOT_NULL(first_argument_expression);
		EXPECT_TRUE(first_argument_expression->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *first_arg_expr_id = first_argument_expression->id;
		EXPECT_NOT_NULL(first_arg_expr_id);
		EXPECT_TRUE(String_equals(first_arg_expr_id->name, "bar"));

		EXPECT_NOT_NULL(first_argument_expression->argumentList)
		Array *arr = (Array*)first_argument_expression->argumentList->arguments;

		EXPECT_NOT_NULL(arr->data);
		EXPECT_TRUE(arr->size == 1);

		// 1
		ArgumentASTNode *first_arg_expr_arg = (ArgumentASTNode*)Array_get(arr, 0);
		EXPECT_NOT_NULL(first_arg_expr_arg);
		EXPECT_NULL(first_arg_expr_arg->label);

		LiteralExpressionASTNode *first_arg_expr_arg_expr = (LiteralExpressionASTNode*)first_arg_expr_arg->expression;
		EXPECT_NOT_NULL(first_arg_expr_arg_expr);
		EXPECT_TRUE(first_arg_expr_arg_expr->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(first_arg_expr_arg_expr->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(first_arg_expr_arg_expr->value.integer, 1);

		// second argument b:10
		argument = (ArgumentASTNode*)Array_get(arguments, 1);
		EXPECT_NOT_NULL(argument);
		EXPECT_NOT_NULL(argument->label);


		IdentifierASTNode *argument_id = argument->label;
		EXPECT_NOT_NULL(argument_id);
		EXPECT_TRUE(String_equals(argument_id->name, "b"));

		LiteralExpressionASTNode *second_argument_expression = (LiteralExpressionASTNode*)argument->expression;
		EXPECT_NOT_NULL(second_argument_expression);
		EXPECT_TRUE(second_argument_expression->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(second_argument_expression->type.type == TYPE_INT);
		EXPECT_EQUAL_INT(second_argument_expression->value.integer, 10);


	} TEST_END();

	TEST_BEGIN("Statement function call with labeled argument") {
		Lexer_setSource(
			&lexer,
			"min(max:low+10)"
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_EXPRESSION_STATEMENT);

		ExpressionStatementASTNode *expression_statement = (ExpressionStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)expression_statement->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "min"));

		EXPECT_NOT_NULL(function_call->argumentList)
		Array *arguments = (Array*)function_call->argumentList->arguments;

		EXPECT_NOT_NULL(arguments->data);
		EXPECT_TRUE(arguments->size == 1);

		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arguments, 0);
		EXPECT_NOT_NULL(argument);
		EXPECT_NOT_NULL(argument->label);

		IdentifierASTNode *argument_id = argument->label;
		EXPECT_NOT_NULL(argument_id);
		EXPECT_TRUE(String_equals(argument_id->name, "max"));

		EXPECT_BINARY_NODE(argument->expression, OPERATOR_PLUS, NODE_IDENTIFIER, NODE_LITERAL_EXPRESSION, binary);
		id = (IdentifierASTNode*)binary->left;
		EXPECT_TRUE(String_equals(id->name, "low"));

	} TEST_END();

	TEST_BEGIN("Unwrap operator containing function call") {
		Lexer_setSource(
			&lexer,
			"a = min()!"
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		UnaryExpressionASTNode *unwrap = (UnaryExpressionASTNode*)assignment->expression;
		EXPECT_NOT_NULL(unwrap);
		EXPECT_TRUE(unwrap->_type == NODE_UNARY_EXPRESSION);

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)unwrap->argument;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "min"));
	} TEST_END();

	TEST_BEGIN("Multiple unwrap operators in a single expression") {
		Lexer_setSource(
			&lexer,
			"a = x! + y!"
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);

		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		EXPECT_BINARY_NODE(assignment->expression, OPERATOR_PLUS, NODE_UNARY_EXPRESSION, NODE_UNARY_EXPRESSION, binary);
	} TEST_END();

	TEST_BEGIN("Function call directly after statement containing function call") {
		Lexer_setSource(
			&lexer,
			"var a = f()" LF
			"f()" LF
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
	} TEST_END();

	TEST_BEGIN("Function call directly after statement containing expression") {
		Lexer_setSource(
			&lexer,
			"var a = 2 + 7" LF
			"f()" LF
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);

		Lexer_setSource(
			&lexer,
			"var a = x" LF
			"f()" LF
		);
		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
	} TEST_END();
}

DESCRIBE(invalid_underscore, "Invalid use of underscore identifier") {

	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Underscore in expression") {
		Lexer_setSource(
			&lexer,
			"var a = 5 + _" LF
		);

		result = Parser_parse(&parser);
		EXPECT_FALSE(result.success);
	} TEST_END();

	TEST_BEGIN("Underscore as function name") {
		Lexer_setSource(
			&lexer,
			"func _() {}" LF
		);

		result = Parser_parse(&parser);
		EXPECT_FALSE(result.success);
	} TEST_END();

	TEST_BEGIN("Underscore as function call") {
		Lexer_setSource(
			&lexer,
			"_(1)" LF
		);

		result = Parser_parse(&parser);
		EXPECT_FALSE(result.success);
	} TEST_END();

	TEST_BEGIN("Underscore as type name") {
		Lexer_setSource(
			&lexer,
			"var a: _ = 5" LF
		);

		result = Parser_parse(&parser);
		EXPECT_FALSE(result.success);
	} TEST_END();

}

DESCRIBE(invalid_equals, "Invalid use of equals sign") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Equals sign in if body") {
		Lexer_setSource(&lexer, "if (true) {=}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in program body") {
		Lexer_setSource(&lexer, "=");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in condition") {
		Lexer_setSource(&lexer, "if = {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in parameter") {
		Lexer_setSource(&lexer, "func (a: Int, =) {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in return") {
		Lexer_setSource(&lexer, "func () -> Int {return = }");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in argument") {
		Lexer_setSource(&lexer, "write(=,\"hello\")");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in varible daclaration expression") {
		Lexer_setSource(&lexer, "let a = =");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in varible daclaration") {
		Lexer_setSource(&lexer, "let = a = 10");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in assignment expression") {
		Lexer_setSource(&lexer, "a = =");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in assignment") {
		Lexer_setSource(&lexer, "= a = 10");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("Equals sign in assignment expression") {
		Lexer_setSource(&lexer, "while (=) {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

}


DESCRIBE(str_interp_parsing, "String interpolation parsing") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Simple string containing no interpolation") {
		Lexer_setSource(
			&lexer,
			"str = \"str\"" LF
		);

		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);
	} TEST_END();

	TEST_BEGIN("Simple string containing single interpolation expression") {
		Lexer_setSource(
			&lexer,
			"str = \"pre \\(expr) post\"" LF
		);

		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);

		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);
		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		InterpolationExpressionASTNode *interpolation = (InterpolationExpressionASTNode*)assignment->expression;
		EXPECT_NOT_NULL(interpolation);
		EXPECT_TRUE(interpolation->_type == NODE_INTERPOLATION_EXPRESSION);

		EXPECT_NOT_NULL(interpolation->strings);
		EXPECT_NOT_NULL(interpolation->expressions);

		EXPECT_EQUAL_INT(interpolation->strings->size, 2);
		EXPECT_EQUAL_INT(interpolation->expressions->size, 1);

		String *string1 = Array_get(interpolation->strings, 0);
		EXPECT_NOT_NULL(string1);
		EXPECT_TRUE(String_equals(string1, "pre "));

		String *string2 = Array_get(interpolation->strings, 1);
		EXPECT_NOT_NULL(string2);
		EXPECT_TRUE(String_equals(string2, " post"));

		IdentifierASTNode *identifier = Array_get(interpolation->expressions, 0);
		EXPECT_NOT_NULL(identifier);
		EXPECT_TRUE(identifier->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(identifier->name, "expr"));
	} TEST_END();

	TEST_BEGIN("Simple string containing multiple interpolation expression") {
		Lexer_setSource(
			&lexer,
			"str = \"pre \\(expr) in \\(expr2) post\"" LF
		);

		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);

		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);
		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		InterpolationExpressionASTNode *interpolation = (InterpolationExpressionASTNode*)assignment->expression;
		EXPECT_NOT_NULL(interpolation);
		EXPECT_TRUE(interpolation->_type == NODE_INTERPOLATION_EXPRESSION);

		EXPECT_NOT_NULL(interpolation->strings);
		EXPECT_NOT_NULL(interpolation->expressions);

		EXPECT_EQUAL_INT(interpolation->strings->size, 3);
		EXPECT_EQUAL_INT(interpolation->expressions->size, 2);

		String *string1 = Array_get(interpolation->strings, 0);
		EXPECT_NOT_NULL(string1);
		EXPECT_TRUE(String_equals(string1, "pre "));

		String *string2 = Array_get(interpolation->strings, 1);
		EXPECT_NOT_NULL(string2);
		EXPECT_TRUE(String_equals(string2, " in "));

		String *string3 = Array_get(interpolation->strings, 2);
		EXPECT_NOT_NULL(string3);
		EXPECT_TRUE(String_equals(string3, " post"));

		IdentifierASTNode *identifier1 = Array_get(interpolation->expressions, 0);
		EXPECT_NOT_NULL(identifier1);
		EXPECT_TRUE(identifier1->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(identifier1->name, "expr"));

		IdentifierASTNode *identifier2 = Array_get(interpolation->expressions, 1);
		EXPECT_NOT_NULL(identifier2);
		EXPECT_TRUE(identifier2->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(identifier2->name, "expr2"));
	} TEST_END();

	TEST_BEGIN("String containing single interpolation string expression") {
		Lexer_setSource(
			&lexer,
			"str = \"pre \\(\"in\") post\"" LF
		);

		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);

		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);
		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		InterpolationExpressionASTNode *interpolation = (InterpolationExpressionASTNode*)assignment->expression;
		EXPECT_NOT_NULL(interpolation);
		EXPECT_TRUE(interpolation->_type == NODE_INTERPOLATION_EXPRESSION);

		EXPECT_NOT_NULL(interpolation->strings);
		EXPECT_NOT_NULL(interpolation->expressions);

		EXPECT_EQUAL_INT(interpolation->strings->size, 2);
		EXPECT_EQUAL_INT(interpolation->expressions->size, 1);

		String *string1 = Array_get(interpolation->strings, 0);
		EXPECT_NOT_NULL(string1);
		EXPECT_TRUE(String_equals(string1, "pre "));

		String *string2 = Array_get(interpolation->strings, 1);
		EXPECT_NOT_NULL(string2);
		EXPECT_TRUE(String_equals(string2, " post"));

		LiteralExpressionASTNode *literal = Array_get(interpolation->expressions, 0);
		EXPECT_NOT_NULL(literal);
		EXPECT_TRUE(literal->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(literal->type.type == TYPE_STRING);
		EXPECT_TRUE(String_equals(literal->value.string, "in"));
	} TEST_END();

	TEST_BEGIN("String containing nested interpolation string expression") {
		Lexer_setSource(
			&lexer,
			"str = \"pre \\(\"in_pre \\(expr) in_post\") post\"" LF
		);

		result = Parser_parse(&parser);
		EXPECT_TRUE(result.success);

		EXPECT_STATEMENT(result.node, NODE_ASSIGNMENT_STATEMENT);
		AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;

		InterpolationExpressionASTNode *interpolation = (InterpolationExpressionASTNode*)assignment->expression;
		EXPECT_NOT_NULL(interpolation);
		EXPECT_TRUE(interpolation->_type == NODE_INTERPOLATION_EXPRESSION);

		EXPECT_NOT_NULL(interpolation->strings);
		EXPECT_NOT_NULL(interpolation->expressions);

		EXPECT_EQUAL_INT(interpolation->strings->size, 2);
		EXPECT_EQUAL_INT(interpolation->expressions->size, 1);

		String *string1 = Array_get(interpolation->strings, 0);
		EXPECT_NOT_NULL(string1);
		EXPECT_TRUE(String_equals(string1, "pre "));

		String *string2 = Array_get(interpolation->strings, 1);
		EXPECT_NOT_NULL(string2);
		EXPECT_TRUE(String_equals(string2, " post"));

		InterpolationExpressionASTNode *interpolation2 = Array_get(interpolation->expressions, 0);
		EXPECT_NOT_NULL(interpolation2);
		EXPECT_TRUE(interpolation2->_type == NODE_INTERPOLATION_EXPRESSION);

		EXPECT_NOT_NULL(interpolation2->strings);
		EXPECT_NOT_NULL(interpolation2->expressions);

		EXPECT_EQUAL_INT(interpolation2->strings->size, 2);
		EXPECT_EQUAL_INT(interpolation2->expressions->size, 1);

		String *string21 = Array_get(interpolation2->strings, 0);
		EXPECT_NOT_NULL(string21);
		EXPECT_TRUE(String_equals(string21, "in_pre "));

		String *string22 = Array_get(interpolation2->strings, 1);
		EXPECT_NOT_NULL(string22);
		EXPECT_TRUE(String_equals(string22, " in_post"));

		IdentifierASTNode *identifier = Array_get(interpolation2->expressions, 0);
		EXPECT_NOT_NULL(identifier);
		EXPECT_TRUE(identifier->_type == NODE_IDENTIFIER);
		EXPECT_TRUE(String_equals(identifier->name, "expr"));
	} TEST_END();


}

DESCRIBE(invalid_lexical, "Lexical error propagation") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("In global scope") {
		Lexer_setSource(&lexer, ".14c20 ");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("In if block") {
		Lexer_setSource(&lexer, "if true { .14 }");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

	TEST_BEGIN("In expression") {
		Lexer_setSource(&lexer, "let a: Int = .10y10");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also
	}
	TEST_END();

}

DESCRIBE(simple_programs, "Simple program parsing") {

	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

	TEST_BEGIN("Empty program") {
		Lexer_setSource(&lexer, "");

		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
	} TEST_END();

	TEST_BEGIN("Factorial iterative") {
		Lexer_setSource(
			&lexer,
			"write(\"Zadejte cislo pro vypocet faktorialu\\n\")" LF
			"let a : Int? = readInt()" LF
			"if let a {" LF
			TAB "if (a < 0) {write(\"Faktorial nelze spocitat\\n\")" LF
			TAB TAB "} else {" LF
			TAB TAB TAB "var a = Int2Double(a)" LF
			TAB TAB TAB "var vysl : Double = 1" LF
			TAB TAB TAB "while (a > 0) {" LF
			TAB TAB TAB TAB "vysl = vysl * a" LF
			TAB TAB TAB TAB "a = a - 1" LF
			TAB TAB TAB "}" LF
			TAB TAB TAB "write(\"Vysledek je: \", vysl, \"\\n\")" LF
			TAB TAB "}" LF
			"} else {" LF
			TAB "write(\"Chyba pri nacitani celeho cisla!\\n\")" LF
			"}" LF
		);

		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENTS(result.node, 3);


		// first statement write(...)
		StatementASTNode *statement = (StatementASTNode*)Array_get(statements, 0);
		EXPECT_TRUE(statement->_type == NODE_EXPRESSION_STATEMENT);

		ExpressionStatementASTNode *expression_statement = (ExpressionStatementASTNode*)statement;

		FunctionCallASTNode *function_call = (FunctionCallASTNode*)expression_statement->expression;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		IdentifierASTNode *id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "write"));

		EXPECT_NOT_NULL(function_call->argumentList)
		Array *arguments = (Array*)function_call->argumentList->arguments;

		EXPECT_NOT_NULL(arguments->data);
		EXPECT_TRUE(arguments->size == 1);

		ArgumentASTNode *argument = (ArgumentASTNode*)Array_get(arguments, 0);
		EXPECT_NOT_NULL(argument);
		EXPECT_NULL(argument->label);

		LiteralExpressionASTNode *argument_expression = (LiteralExpressionASTNode*)argument->expression;
		EXPECT_NOT_NULL(argument_expression);
		EXPECT_TRUE(argument_expression->_type == NODE_LITERAL_EXPRESSION);
		EXPECT_TRUE(argument_expression->type.type == TYPE_STRING);
		EXPECT_TRUE(String_equals(argument_expression->value.string, "Zadejte cislo pro vypocet faktorialu\n"));


		// statement let a: Int? = ...
		statement = (StatementASTNode*)Array_get(statements, 1);
		EXPECT_TRUE(statement->_type == NODE_VARIABLE_DECLARATION);
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
		EXPECT_TRUE(String_equals(pattern->type->id->name, "Int"));
		EXPECT_TRUE(pattern->type->isNullable);

		id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "a"));

		// readInt()
		function_call = (FunctionCallASTNode*)declarator->initializer;
		EXPECT_NOT_NULL(function_call);
		EXPECT_TRUE(function_call->_type == NODE_FUNCTION_CALL);

		id = function_call->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "readInt"));

		EXPECT_NOT_NULL(function_call->argumentList)
		arguments = (Array*)function_call->argumentList->arguments;

		EXPECT_NULL(arguments->data);
		EXPECT_TRUE(arguments->size == 0);


		// if let a { ...
		statement = (StatementASTNode*)Array_get(statements, 2);
		EXPECT_TRUE(statement->_type == NODE_IF_STATEMENT);

		IfStatementASTNode *if_statement = (IfStatementASTNode*)statement;

		EXPECT_NOT_NULL(if_statement->test);
		EXPECT_TRUE(if_statement->test->_type == NODE_OPTIONAL_BINDING_CONDITION);


	} TEST_END();

}


