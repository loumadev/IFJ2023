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
	return node;
}

IdentifierASTNode* new_IdentifierASTNode(
	String *name
) {
	prepare_node_of(IdentifierASTNode, NODE_IDENTIFIER)
	node->name = name;
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

VariableDeclarationASTNode* new_VariableDeclarationASTNode(
	IdentifierASTNode *id,
	TypeReferenceASTNode *type
) {
	prepare_node_of(VariableDeclarationASTNode, NODE_VARIABLE_DECLARATION)
	node->id = id;
	node->type = type;
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
	return node;
}

ParameterASTNode* new_ParameterASTNode(
	IdentifierASTNode *id,
	TypeReferenceASTNode *type,
	ExpressionASTNode *initializer,
	IdentifierASTNode *externalName,
	bool isLabeless
) {
	prepare_node_of(ParameterASTNode, NODE_PARAMETER)
	node->id = id;
	node->type = type;
	node->initializer = initializer;
	node->externalName = externalName;
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

ArgumentASTNode* new_ArgumentASTNode(
	ExpressionASTNode *expression,
	IdentifierASTNode *label
) {
	prepare_node_of(ArgumentASTNode, NODE_ARGUMENT)
	node->label = label;
	node->expression = expression;
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
	return node;
}

FunctionCallASTNode* new_FunctionCallASTNode(
	IdentifierASTNode *id,
	Array *arguments
) {
	prepare_node_of(FunctionCallASTNode, NODE_FUNCTION_CALL)
	node->id = id;
	node->arguments = arguments;
	return node;
}

BinaryExpressionASTNode* new_BinaryExpressionASTNode( 
	ExpressionASTNode *left, 
	ExpressionASTNode *right, 
	enum OperatorType operator
){
		prepare_node_of(BinaryExpressionASTNode, NODE_BINARY_EXPRESSION)
		node->left = left;
		node->right = right;
		node->operator = operator;
		return node;
}

UnaryExpressionASTNode* new_UnaryExpressionASTNode( 
	ExpressionASTNode *argument, 
	enum OperatorType operator
	//bool IsPrefix
) {
		prepare_node_of(UnaryExpressionASTNode, NODE_UNARY_EXPRESSION)
		node->argument = argument;
		node->operator = operator;
		//node->IsPrefix = IsPrefix;
		return node;
}

LiteralExpressionASTNode* new_LiteralExpressionASTNode(
	union TokenValue value
) {
	prepare_node_of(LiteralExpressionASTNode, NODE_LITERAL_EXPRESSION)
	node->value = value;
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
