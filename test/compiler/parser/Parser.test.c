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
		EXPECT_TRUE(String_equals(pattern->type->id->name, "String"));

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
		EXPECT_TRUE(String_equals(pattern->type->id->name, "Int"));

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
		EXPECT_TRUE(initializer->type == LITERAL_INTEGER);
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
		EXPECT_TRUE(initializer->type == LITERAL_STRING);
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
		EXPECT_TRUE(initializer->type == LITERAL_INTEGER);
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
		EXPECT_TRUE(initializer->type == LITERAL_FLOATING);
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
		EXPECT_TRUE(condition_expression->type == LITERAL_BOOLEAN);
		EXPECT_TRUE(condition_expression->value.boolean);

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else
		EXPECT_NULL(if_statement->elseClause);

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
		EXPECT_TRUE(condition_expression->type == LITERAL_BOOLEAN);
		EXPECT_TRUE(condition_expression->value.boolean);

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else
		EXPECT_NOT_NULL(if_statement->elseClause);
		EXPECT_FALSE(if_statement->elseClause->isElseIf);
		EXPECT_NULL(if_statement->elseClause->ifStatement);
		EXPECT_NOT_NULL(if_statement->elseClause->body);

		// else body
		body = if_statement->elseClause->body;
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
		EXPECT_TRUE(right->type == LITERAL_INTEGER);
		EXPECT_EQUAL_INT(right->value.integer, 10);

		EXPECT_TRUE(condition_expression->operator == OPERATOR_GREATER);

		// if body
		BlockASTNode *body = if_statement->body;
		EXPECT_NOT_NULL(body->statements);
		Array *arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else if
		EXPECT_NOT_NULL(if_statement->elseClause);
		EXPECT_TRUE(if_statement->elseClause->isElseIf);
		EXPECT_NOT_NULL(if_statement->elseClause->ifStatement);

		IfStatementASTNode *elseif = if_statement->elseClause->ifStatement;

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
		EXPECT_TRUE(right->type == LITERAL_INTEGER);
		EXPECT_EQUAL_INT(right->value.integer, 10);

		EXPECT_TRUE(condition_expression->operator == OPERATOR_LESS);

		// else if body
		body = elseif->body;
		EXPECT_NOT_NULL(body->statements);
		arr = body->statements;
		EXPECT_NULL(arr->data);
		EXPECT_EQUAL_INT(arr->size, 0);

		// else
		EXPECT_NOT_NULL(elseif->elseClause);
		EXPECT_FALSE(elseif->elseClause->isElseIf);
		EXPECT_NULL(elseif->elseClause->ifStatement);
		EXPECT_NOT_NULL(elseif->elseClause->body);

		// else body
		body = elseif->elseClause->body;
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
		EXPECT_NULL(if_statement->elseClause);

	} TEST_END();
}

