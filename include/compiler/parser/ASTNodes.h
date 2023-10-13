#include <stdbool.h>

#include "internal/Array.h"
#include "internal/String.h"

#ifndef ASTNode_H
#define ASTNode_H

enum ASTNodeType {
	NODE_INVALID = 0,
	NODE_PROGRAM,
	NODE_BLOCK,
	NODE_IDENTIFIER,
	NODE_TYPE_REFERENCE,
	NODE_VARIABLE_DECLARATION,
	NODE_EXPRESSION_STATEMENT,
	NODE_RETURN_STATEMENT,
	NODE_PARAMETER,
	NODE_PARAMETER_LIST,
	NODE_FUNCTION_DECLARATION,
	NODE_ARGUMENT,
	NODE_FUNCTION_CALL
};


/* Definition of AST nodes */

typedef struct ASTNode {
	enum ASTNodeType _type;
} ASTNode;

typedef ASTNode ExpressionASTNode;
typedef ASTNode StatementASTNode;

typedef struct BlockASTNode {
	enum ASTNodeType _type;
	Array /*<StatementASTNode>*/ *statements;
} BlockASTNode;

typedef struct ProgramASTNode {
	enum ASTNodeType _type;
	BlockASTNode *block;
} ProgramASTNode;

typedef struct IdentifierASTNode {
	enum ASTNodeType _type;
	String *name;
} IdentifierASTNode;

typedef struct TypeReferenceASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	bool isNullable;
} TypeReferenceASTNode;

typedef struct VariableDeclarationASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	TypeReferenceASTNode *type;
} VariableDeclarationASTNode;

typedef struct ExpressionStatementASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *expression;
} ExpressionStatementASTNode;

typedef struct ReturnStatementASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *expression;
} ReturnStatementASTNode;

typedef struct ParameterASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	TypeReferenceASTNode *type;
	ExpressionASTNode *initializer;
	IdentifierASTNode *externalName;
	bool isLabeless;
} ParameterASTNode;

typedef struct ParameterListASTNode {
	enum ASTNodeType _type;
	Array /*<ParameterASTNode>*/ *parameters;
} ParameterListASTNode;

typedef struct ArgumentASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *expression;
	IdentifierASTNode *label;
} ArgumentASTNode;

typedef struct FunctionDeclarationASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	ParameterListASTNode *parameterList;
	TypeReferenceASTNode *returnType;
	BlockASTNode *body;
} FunctionDeclarationASTNode;

typedef struct FunctionCallASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	Array /*<ArgumentASTNode>*/ *arguments;
} FunctionCallASTNode;

// TODO: Add more AST nodes


/* Constructors for AST nodes */

ProgramASTNode * new_ProgramASTNode(BlockASTNode *block);
BlockASTNode * new_BlockASTNode(Array *statements);
IdentifierASTNode * new_IdentifierASTNode(String *name);
TypeReferenceASTNode * new_TypeReferenceASTNode(IdentifierASTNode *id, bool isNullable);
VariableDeclarationASTNode * new_VariableDeclarationASTNode(IdentifierASTNode *id, TypeReferenceASTNode *type);
ReturnStatementASTNode * new_ReturnStatementASTNode(ExpressionASTNode *expression);
ParameterASTNode * new_ParameterASTNode(IdentifierASTNode *id, TypeReferenceASTNode *type, ExpressionASTNode *initializer, IdentifierASTNode *externalName, bool isLabeless);
ParameterListASTNode * new_ParameterListASTNode(Array *parameters);
ArgumentASTNode * new_ArgumentASTNode(ExpressionASTNode *expression, IdentifierASTNode *label);
FunctionDeclarationASTNode * new_FunctionDeclarationASTNode(IdentifierASTNode *id, ParameterListASTNode *parameterList, TypeReferenceASTNode *returnType, BlockASTNode *body);
FunctionCallASTNode * new_FunctionCallASTNode(IdentifierASTNode *id, Array *arguments);

// TODO: Add more AST node constructors


/* Other public functions */

ASTNode * ASTNode_alloc(size_t size, enum ASTNodeType type);
void ASTNode_free(ASTNode *node);

void ASTNode_print(ASTNode *node);

#define prepare_node_of(dataType, nodeType) dataType *node = (dataType*)ASTNode_alloc(sizeof(dataType), nodeType);

#endif
