/**
 * @file src/compiler/parser/ASTNodes.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @author Veronika Krobotová <xkrobo03@stud.fit.vut.cz>
 * @author Radim Mifka <xmifka00@stud.fit.vut.cz>
 * @brief Implementation of nodes for abstract syntax tree.
 * @copyright Copyright (c) 2023
 */

#include "compiler/parser/ASTNodes.h"

#include "assertf.h"

#include "allocator/MemoryAllocator.h"


/* Definitions of ASTNodes */

ProgramASTNode* new_ProgramASTNode(
	BlockASTNode *block
) {
	prepare_node_of(ProgramASTNode, NODE_PROGRAM)
	node->block = block;
	return node;
}

BlockASTNode* new_BlockASTNode(
	Array *statements
) {
	prepare_node_of(BlockASTNode, NODE_BLOCK)
	node->statements = statements;
	node->scope = NULL;
	return node;
}

IdentifierASTNode* new_IdentifierASTNode(
	String *name
) {
	prepare_node_of(IdentifierASTNode, NODE_IDENTIFIER)
	node->name = name;
	node->id = 0;
	return node;
}

TypeReferenceASTNode* new_TypeReferenceASTNode(
	IdentifierASTNode *id,
	bool isNullable
) {
	prepare_node_of(TypeReferenceASTNode, NODE_TYPE_REFERENCE)
	node->id = id;
	node->isNullable = isNullable;
	return node;
}

VariableDeclaratorASTNode* new_VariableDeclaratorASTNode(
	PatternASTNode *pattern,
	ExpressionASTNode *initializer
) {
	prepare_node_of(VariableDeclaratorASTNode, NODE_VARIABLE_DECLARATOR)
	node->pattern = pattern;
	node->initializer = initializer;
	return node;
}

VariableDeclarationListASTNode* new_VariableDeclarationListASTNode(
	Array *declarators
) {
	prepare_node_of(VariableDeclarationListASTNode, NODE_VARIABLE_DECLARATION_LIST)
	node->declarators = declarators;
	return node;
}

VariableDeclarationASTNode* new_VariableDeclarationASTNode(
	VariableDeclarationListASTNode *declaratorList,
	bool isConstant
) {
	prepare_node_of(VariableDeclarationASTNode, NODE_VARIABLE_DECLARATION)
	node->declaratorList = declaratorList;
	node->isConstant = isConstant;
	return node;
}


ExpressionStatementASTNode* new_ExpressionStatementASTNode(
	ExpressionASTNode *expression
) {
	prepare_node_of(ExpressionStatementASTNode, NODE_EXPRESSION_STATEMENT)
	node->expression = expression;
	return node;
}

ReturnStatementASTNode* new_ReturnStatementASTNode(
	ExpressionASTNode *expression
) {
	prepare_node_of(ReturnStatementASTNode, NODE_RETURN_STATEMENT)
	node->expression = expression;
	node->id = 0;
	return node;
}

BreakStatementASTNode* new_BreakStatementASTNode() {
	prepare_node_of(BreakStatementASTNode, NODE_BREAK_STATEMENT)
	node->id = 0;
	return node;
}

ContinueStatementASTNode* new_ContinueStatementASTNode() {
	prepare_node_of(ContinueStatementASTNode, NODE_CONTINUE_STATEMENT)
	node->id = 0;
	return node;
}

ParameterASTNode* new_ParameterASTNode(
	IdentifierASTNode *internalId,
	TypeReferenceASTNode *type,
	ExpressionASTNode *initializer,
	IdentifierASTNode *externalId,
	bool isLabeless
) {
	prepare_node_of(ParameterASTNode, NODE_PARAMETER)
	node->internalId = internalId;
	node->type = type;
	node->initializer = initializer;
	node->externalId = externalId;
	node->isLabeless = isLabeless;
	return node;
}

ParameterListASTNode* new_ParameterListASTNode(
	Array *parameters
) {
	prepare_node_of(ParameterListASTNode, NODE_PARAMETER_LIST)
	node->parameters = parameters;
	return node;
}

FunctionDeclarationASTNode* new_FunctionDeclarationASTNode(
	IdentifierASTNode *id,
	ParameterListASTNode *parameterList,
	TypeReferenceASTNode *returnType,
	BlockASTNode *block
) {
	prepare_node_of(FunctionDeclarationASTNode, NODE_FUNCTION_DECLARATION)
	node->id = id;
	node->parameterList = parameterList;
	node->returnType = returnType;
	node->body = block;
	node->builtin = FUNCTION_NONE;
	return node;
}

ArgumentASTNode* new_ArgumentASTNode(
	ExpressionASTNode *expression,
	IdentifierASTNode *label
) {
	prepare_node_of(ArgumentASTNode, NODE_ARGUMENT)
	node->label = label;
	node->expression = expression;
	return node;
}

ArgumentListASTNode* new_ArgumentListASTNode(
	Array *arguments
) {
	prepare_node_of(ArgumentListASTNode, NODE_ARGUMENT_LIST)
	node->arguments = arguments;
	return node;
}

FunctionCallASTNode* new_FunctionCallASTNode(
	IdentifierASTNode *id,
	ArgumentListASTNode *argumentList
) {
	prepare_node_of(FunctionCallASTNode, NODE_FUNCTION_CALL)
	node->id = id;
	node->argumentList = argumentList;
	return node;
}

PatternASTNode* new_PatternASTNode(
	IdentifierASTNode *id,
	TypeReferenceASTNode *type
) {
	prepare_node_of(PatternASTNode, NODE_PATTERN)
	node->id = id;
	node->type = type;
	return node;
}

OptionalBindingConditionASTNode* new_OptionalBindingConditionASTNode(
	IdentifierASTNode *id
) {
	prepare_node_of(OptionalBindingConditionASTNode, NODE_OPTIONAL_BINDING_CONDITION)
	node->id = id;
	node->fromId = 0;
	return node;
}

IfStatementASTNode* new_IfStatementASTNode(
	ASTNode *test,
	BlockASTNode *body,
	ASTNode *alternate
) {
	prepare_node_of(IfStatementASTNode, NODE_IF_STATEMENT)
	node->test = test;
	node->body = body;
	node->alternate = alternate;
	node->id = 0;
	return node;
}

WhileStatementASTNode* new_WhileStatementASTNode(
	ASTNode *test,
	BlockASTNode *body
) {
	prepare_node_of(WhileStatementASTNode, NODE_WHILE_STATEMENT)
	node->test = test;
	node->body = body;
	node->id = 0;
	return node;
}

ForStatementASTNode* new_ForStatementASTNode(IdentifierASTNode *iterator, RangeASTNode *range, BlockASTNode *body) {
	prepare_node_of(ForStatementASTNode, NODE_FOR_STATEMENT)
	node->iterator = iterator;
	node->range = range;
	node->body = body;
	node->id = 0;
	return node;
}

RangeASTNode* new_RangeASTNode(ExpressionASTNode *start, ExpressionASTNode *end, OperatorType operator) {
	prepare_node_of(RangeASTNode, NODE_RANGE)
	node->start = start;
	node->end = end;
	node->operator = operator;
	node->endId = 0;
	return node;
}

AssignmentStatementASTNode* new_AssignmentStatementASTNode(
	IdentifierASTNode *id,
	ExpressionASTNode *expression
) {
	prepare_node_of(AssignmentStatementASTNode, NODE_ASSIGNMENT_STATEMENT)
	node->id = id;
	node->expression = expression;
	return node;
}

BinaryExpressionASTNode* new_BinaryExpressionASTNode(
	ExpressionASTNode *left,
	ExpressionASTNode *right,
	OperatorType operator
) {
	prepare_node_of(BinaryExpressionASTNode, NODE_BINARY_EXPRESSION)
	node->left = left;
	node->right = right;
	node->operator = operator;
	return node;
}

UnaryExpressionASTNode* new_UnaryExpressionASTNode(
	ExpressionASTNode *argument,
	OperatorType operator,
	bool isPrefix
) {
	prepare_node_of(UnaryExpressionASTNode, NODE_UNARY_EXPRESSION)
	node->argument = argument;
	node->operator = operator;
	node->isPrefix = isPrefix;
	return node;
}

LiteralExpressionASTNode* new_LiteralExpressionASTNode(
	ValueType type,
	union TokenValue value
) {
	prepare_node_of(LiteralExpressionASTNode, NODE_LITERAL_EXPRESSION)
	node->type = type;
	node->value = value;
	node->originalType = type;
	node->originalValue = value;
	return node;
}

InterpolationExpressionASTNode* new_InterpolationExpressionASTNode(
	Array /*<String>*/ *strings,
	Array /*<ExpressionASTNode>*/ *expressions
) {
	prepare_node_of(InterpolationExpressionASTNode, NODE_INTERPOLATION_EXPRESSION)
	node->strings = strings;
	node->expressions = expressions;
	node->concatenated = NULL;
	return node;
}

// TODO: Add more ASTNode constructors


/* General purpose methods */

ASTNode* ASTNode_alloc(size_t size, enum ASTNodeType type) {
	ASTNode *node = mem_alloc(size);
	node->_type = type;
	return node;
}

void ASTNode_free(ASTNode *node) {
	mem_free(node);
}

void ASTNode_print(ASTNode *node) {

	(void)node;

	// TODO: Implement this
	// assertf(0, "ASTNode_print not implemented");
}

/** End of file src/compiler/parser/ASTNodes.c **/
