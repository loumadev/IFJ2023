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
	NODE_VARIABLE_DECLARATION_LIST,
	NODE_VARIABLE_DECLARATOR,
	NODE_EXPRESSION_STATEMENT,
	NODE_RETURN_STATEMENT,
	NODE_PARAMETER,
	NODE_PARAMETER_LIST,
	NODE_FUNCTION_DECLARATION,
	NODE_ARGUMENT,
	NODE_ARGUMENT_LIST,
	NODE_FUNCTION_CALL,
	NODE_IF_STATEMENT,
	NODE_ELSE_CLAUSE,
	NODE_PATTERN,
	NODE_CONDITION,
	NODE_OPTIONAL_BINDING_CONDITION,
	NODE_WHILE_STATEMENT,
	NODE_ASSIGNMENT_STATEMENT
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

typedef struct VariableDeclaratorASTNode {
	enum ASTNodeType _type;
	struct PatternASTNode *pattern;
	ExpressionASTNode *initializer;
} VariableDeclaratorASTNode;

typedef struct VariableDeclarationListASTNode {
	enum ASTNodeType _type;
	Array /*<VariableDeclaratorASTNode>*/ *declarators;
} VariableDeclarationListASTNode;

typedef struct VariableDeclarationASTNode {
	enum ASTNodeType _type;
	VariableDeclarationListASTNode *declaratorList;
	bool isConstant;
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

typedef struct FunctionDeclarationASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	ParameterListASTNode *parameterList;
	TypeReferenceASTNode *returnType;
	BlockASTNode *body;
} FunctionDeclarationASTNode;

typedef struct ArgumentASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *expression;
	IdentifierASTNode *label;
} ArgumentASTNode;

typedef struct ArgumentListASTNode {
	enum ASTNodeType _type;
	Array /*<ArgumentASTNode>*/ *arguments;
} ArgumentListASTNode;

typedef struct FunctionCallASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	ArgumentListASTNode *argumentList;
} FunctionCallASTNode;

typedef struct PatternASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *name;
	TypeReferenceASTNode *type;
} PatternASTNode;

typedef struct OptionalBindingConditionASTNode {
	enum ASTNodeType _type;
	PatternASTNode *pattern;
	ExpressionASTNode *initializer;
	bool isConstant;
} OptionalBindingConditionASTNode;

typedef struct ConditionASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *expression;
	OptionalBindingConditionASTNode *optionalBindingCondition;
} ConditionASTNode;

typedef struct ElseClauseASTNode {
	enum ASTNodeType _type;
	struct IfStatementASTNode *ifStatement;
	BlockASTNode *body;
	bool isElseIf;
} ElseClauseASTNode;

typedef struct IfStatementASTNode {
	enum ASTNodeType _type;
	ConditionASTNode *condition;
	BlockASTNode *body;
	ElseClauseASTNode *elseClause;
} IfStatementASTNode;

typedef struct WhileStatementASTNode {
	enum ASTNodeType _type;
	ConditionASTNode *condition;
	BlockASTNode *body;
} WhileStatementASTNode;

typedef struct AssignmentStatementASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	ExpressionASTNode *assignment;
} AssignmentStatementASTNode;

// TODO: Add more AST nodes


/* Constructors for AST nodes */

ProgramASTNode * new_ProgramASTNode(BlockASTNode *block);
BlockASTNode * new_BlockASTNode(Array *statements);
IdentifierASTNode * new_IdentifierASTNode(String *name);
TypeReferenceASTNode * new_TypeReferenceASTNode(IdentifierASTNode *id, bool isNullable);
VariableDeclarationASTNode * new_VariableDeclarationASTNode(VariableDeclarationListASTNode *declaratorList, bool isConstant);
VariableDeclaratorASTNode * new_VariableDeclaratorASTNode(PatternASTNode *pattern, ExpressionASTNode *initializer);
VariableDeclarationListASTNode * new_VariableDeclarationListASTNode(Array *declarators);
ReturnStatementASTNode * new_ReturnStatementASTNode(ExpressionASTNode *expression);
ParameterASTNode * new_ParameterASTNode(IdentifierASTNode *id, TypeReferenceASTNode *type, ExpressionASTNode *initializer, IdentifierASTNode *externalName, bool isLabeless);
ParameterListASTNode * new_ParameterListASTNode(Array *parameters);
FunctionDeclarationASTNode * new_FunctionDeclarationASTNode(IdentifierASTNode *id, ParameterListASTNode *parameterList, TypeReferenceASTNode *returnType, BlockASTNode *body);
ArgumentASTNode * new_ArgumentASTNode(ExpressionASTNode *expression, IdentifierASTNode *label);
ArgumentListASTNode * new_ArgumentListASTNode(Array *arguments);
FunctionCallASTNode * new_FunctionCallASTNode(IdentifierASTNode *id, ArgumentListASTNode *argumentList);
PatternASTNode * new_PatternASTNode(IdentifierASTNode *name, TypeReferenceASTNode *type);
OptionalBindingConditionASTNode * new_OptionalBindingConditionASTNode(PatternASTNode *pattern, ExpressionASTNode *initializer, bool isConstant);
ConditionASTNode * new_ConditionASTNode(ExpressionASTNode *expression, OptionalBindingConditionASTNode *optionalBindingCondition);
ElseClauseASTNode * new_ElseClauseASTNode(IfStatementASTNode *ifStatement, BlockASTNode *body, bool isElseIf);
IfStatementASTNode * new_IfStatementASTNode(ConditionASTNode *condition,  BlockASTNode *body, ElseClauseASTNode *elseClause);
WhileStatementASTNode * new_WhileStatementASTNode(ConditionASTNode *condition,  BlockASTNode *body);
AssignmentStatementASTNode * new_AssignmentStatementASTNode(IdentifierASTNode *id, ExpressionASTNode *assignment);

// TODO: Add more AST node constructors


/* Other public functions */

ASTNode * ASTNode_alloc(size_t size, enum ASTNodeType type);
void ASTNode_free(ASTNode *node);

void ASTNode_print(ASTNode *node);

#define prepare_node_of(dataType, nodeType) dataType *node = (dataType*)ASTNode_alloc(sizeof(dataType), nodeType);

#endif
