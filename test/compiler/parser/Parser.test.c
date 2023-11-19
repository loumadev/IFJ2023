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

		EXPECT_NOT_NULL(if_statement->condition);
		EXPECT_TRUE(if_statement->condition->_type == NODE_CONDITION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)if_statement->condition->expression;
		EXPECT_NOT_NULL(condition_expression);
		EXPECT_TRUE(condition_expression->_type == NODE_LITERAL_EXPRESSION);
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

		EXPECT_NOT_NULL(if_statement->condition);
		EXPECT_TRUE(if_statement->condition->_type == NODE_CONDITION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)if_statement->condition->expression;
		EXPECT_NOT_NULL(condition_expression);
		EXPECT_TRUE(condition_expression->_type == NODE_LITERAL_EXPRESSION);
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

		EXPECT_NOT_NULL(if_statement->condition);
		EXPECT_TRUE(if_statement->condition->_type == NODE_CONDITION);

		BinaryExpressionASTNode *condition_expression = (BinaryExpressionASTNode*)if_statement->condition->expression;
		EXPECT_NOT_NULL(condition_expression);
		EXPECT_TRUE(condition_expression->_type == NODE_BINARY_EXPRESSION);

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

		EXPECT_NOT_NULL(elseif->condition);
		EXPECT_TRUE(elseif->condition->_type == NODE_CONDITION);

		condition_expression = (BinaryExpressionASTNode*)elseif->condition->expression;
		EXPECT_NOT_NULL(condition_expression);
		EXPECT_TRUE(condition_expression->_type == NODE_BINARY_EXPRESSION);

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
		Lexer_setSource(&lexer, "if (let b = a) {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition no body, no else") {
		Lexer_setSource(&lexer, "if let b = a {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_IF_STATEMENT);

		IfStatementASTNode *if_statement = (IfStatementASTNode*)statement;

		EXPECT_NOT_NULL(if_statement->condition);
		EXPECT_TRUE(if_statement->condition->_type == NODE_CONDITION);

		EXPECT_NULL(if_statement->condition->expression);

		OptionalBindingConditionASTNode *binding_condition = (OptionalBindingConditionASTNode*)if_statement->condition->optionalBindingCondition;
		EXPECT_NOT_NULL(binding_condition);
		EXPECT_TRUE(binding_condition->isConstant);
		EXPECT_TRUE(binding_condition->_type == NODE_OPTIONAL_BINDING_CONDITION);

		PatternASTNode *pattern = binding_condition->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NULL(pattern->type);

		IdentifierASTNode *id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "b"));

		IdentifierASTNode *initializer = (IdentifierASTNode*)binding_condition->initializer;

		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_IDENTIFIER);
		EXPECT_NOT_NULL(initializer->name);
		EXPECT_TRUE(String_equals(initializer->name, "a"));

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

		EXPECT_NOT_NULL(while_statement->condition);
		EXPECT_TRUE(while_statement->condition->_type == NODE_CONDITION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)while_statement->condition->expression;
		EXPECT_NOT_NULL(condition_expression);
		EXPECT_TRUE(condition_expression->_type == NODE_LITERAL_EXPRESSION);
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

		EXPECT_NOT_NULL(while_statement->condition);
		EXPECT_TRUE(while_statement->condition->_type == NODE_CONDITION);

		LiteralExpressionASTNode *condition_expression = (LiteralExpressionASTNode*)while_statement->condition->expression;
		EXPECT_NOT_NULL(condition_expression);
		EXPECT_TRUE(condition_expression->_type == NODE_LITERAL_EXPRESSION);
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
		Lexer_setSource(&lexer, "while (let hello = world) {}");
		result = Parser_parse(&parser);

		EXPECT_FALSE(result.success);
		EXPECT_NULL(result.node);

		EXPECT_TRUE(result.type == RESULT_ERROR_SYNTACTIC_ANALYSIS);
		EXPECT_TRUE(result.severity == SEVERITY_ERROR);
		// prbbly later add message check also

	} TEST_END();

	TEST_BEGIN("Binding condition no body") {
		Lexer_setSource(&lexer, "while let hello = world {}");
		result = Parser_parse(&parser);

		EXPECT_TRUE(result.success);
		EXPECT_STATEMENT(result.node, NODE_WHILE_STATEMENT);

		WhileStatementASTNode *while_statement = (WhileStatementASTNode*)statement;

		EXPECT_NOT_NULL(while_statement->condition);
		EXPECT_TRUE(while_statement->condition->_type == NODE_CONDITION);

		EXPECT_NULL(while_statement->condition->expression);

		OptionalBindingConditionASTNode *binding_condition = (OptionalBindingConditionASTNode*)while_statement->condition->optionalBindingCondition;
		EXPECT_NOT_NULL(binding_condition);
		EXPECT_TRUE(binding_condition->isConstant);
		EXPECT_TRUE(binding_condition->_type == NODE_OPTIONAL_BINDING_CONDITION);

		PatternASTNode *pattern = binding_condition->pattern;
		EXPECT_NOT_NULL(pattern);
		EXPECT_NULL(pattern->type);

		IdentifierASTNode *id = pattern->id;
		EXPECT_NOT_NULL(id);
		EXPECT_TRUE(String_equals(id->name, "hello"));

		IdentifierASTNode *initializer = (IdentifierASTNode*)binding_condition->initializer;

		EXPECT_NOT_NULL(initializer);
		EXPECT_TRUE(initializer->_type == NODE_IDENTIFIER);
		EXPECT_NOT_NULL(initializer->name);
		EXPECT_TRUE(String_equals(initializer->name, "world"));

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
}
