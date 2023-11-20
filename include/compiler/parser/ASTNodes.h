#include <stdbool.h>

#include "internal/Array.h"
#include "internal/String.h"

#include "compiler/lexer/Token.h"
#include "compiler/analyser/Analyser.h"

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
	NODE_BINARY_EXPRESSION,
	NODE_UNARY_EXPRESSION,
	NODE_LITERAL_EXPRESSION,
	NODE_ARGUMENT_LIST,
	NODE_FUNCTION_CALL,
	NODE_IF_STATEMENT,
	NODE_PATTERN,
	NODE_OPTIONAL_BINDING_CONDITION,
	NODE_WHILE_STATEMENT,
	NODE_ASSIGNMENT_STATEMENT
};

typedef enum OperatorType {
	OPERATOR_DEFAULT = 0,
	OPERATOR_PLUS,
	OPERATOR_MINUS,
	OPERATOR_MUL,
	OPERATOR_DIV,
	OPERATOR_UNWRAP,
	OPERATOR_NULL_COALESCING,
	OPERATOR_EQUAL,
	OPERATOR_NOT_EQUAL,
	OPERATOR_LESS,
	OPERATOR_GREATER,
	OPERATOR_LESS_EQUAL,
	OPERATOR_GREATER_EQUAL
} OperatorType;

enum BuiltInTypes {
	TYPE_NIL = -2,
	TYPE_UNKNOWN = -1,
	TYPE_INVALID = 0,
	TYPE_INT,
	TYPE_DOUBLE,
	TYPE_BOOL,
	TYPE_STRING,
	TYPE_VOID
};

#define is_type_valid(type) ((type) > TYPE_INVALID)
#define is_value_assignable(dst, src) (((dst).type == (src).type || (src).type == TYPE_NIL) && ((dst).isNullable || !(src).isNullable))

typedef struct ValueType {
	enum BuiltInTypes type;
	bool isNullable;
} ValueType;


/* Definition of AST nodes */

typedef struct ASTNode {
	enum ASTNodeType _type;
} ASTNode;

typedef ASTNode ExpressionASTNode;
typedef ASTNode StatementASTNode;

typedef struct BlockASTNode {
	enum ASTNodeType _type;
	Array /*<StatementASTNode>*/ *statements;
	struct BlockScope *scope;
} BlockASTNode;

typedef struct ProgramASTNode {
	enum ASTNodeType _type;
	BlockASTNode *block;
} ProgramASTNode;

typedef struct IdentifierASTNode {
	enum ASTNodeType _type;
	String *name;
	size_t id;
} IdentifierASTNode;

typedef struct TypeReferenceASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	bool isNullable;
	struct ValueType type;
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
	IdentifierASTNode *internalId;
	TypeReferenceASTNode *type;
	ExpressionASTNode *initializer;
	IdentifierASTNode *externalId;
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

// typedef struct ExpressionASTNode {
//	enum ASTNodeType _type;
//	BinaryExpressionASTNode *BExpression;
//	UnaryExpressionASTNode *UExpression;
//	LiteralExpressionASTNode *LExpression;
//	IdentifierASTNode *IExpression;
// } ExpressionASTNode;

typedef struct BinaryExpressionASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *left;
	ExpressionASTNode *right;
	OperatorType operator;
	struct ValueType type;
} BinaryExpressionASTNode;

typedef struct UnaryExpressionASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *argument;
	OperatorType operator;
	// bool isPrefix;
	struct ValueType type;
} UnaryExpressionASTNode;

typedef struct LiteralExpressionASTNode {
	enum ASTNodeType _type;
	union TokenValue value;
	struct ValueType type;
} LiteralExpressionASTNode;

typedef struct PatternASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	TypeReferenceASTNode *type;
} PatternASTNode;

typedef struct OptionalBindingConditionASTNode {
    enum ASTNodeType _type;
    IdentifierASTNode *id;
} OptionalBindingConditionASTNode;

typedef struct IfStatementASTNode {
	enum ASTNodeType _type;
    ASTNode /* <ExpressionASTNode | OptionalBindingConditionASTNode> */ *test;
	BlockASTNode *body;
	ASTNode /* BlockASTNode | IfStatementASTNode | null */ *alternate;
} IfStatementASTNode;

typedef struct WhileStatementASTNode {
	enum ASTNodeType _type;
    ASTNode /* <ExpressionASTNode | OptionalBindingConditionASTNode> */ *test;
	BlockASTNode *body;
} WhileStatementASTNode;

typedef struct AssignmentStatementASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	ExpressionASTNode *expression;
} AssignmentStatementASTNode;

// TODO: Add more AST nodes


/* Constructors for AST nodes */

ProgramASTNode* new_ProgramASTNode(BlockASTNode *block);
BlockASTNode* new_BlockASTNode(Array *statements);
IdentifierASTNode* new_IdentifierASTNode(String *name);
TypeReferenceASTNode* new_TypeReferenceASTNode(IdentifierASTNode *id, bool isNullable);
VariableDeclarationASTNode* new_VariableDeclarationASTNode(VariableDeclarationListASTNode *declaratorList, bool isConstant);
VariableDeclaratorASTNode* new_VariableDeclaratorASTNode(PatternASTNode *pattern, ExpressionASTNode *initializer);
VariableDeclarationListASTNode* new_VariableDeclarationListASTNode(Array *declarators);
ReturnStatementASTNode* new_ReturnStatementASTNode(ExpressionASTNode *expression);
ParameterASTNode* new_ParameterASTNode(IdentifierASTNode *internalId, TypeReferenceASTNode *type, ExpressionASTNode *initializer, IdentifierASTNode *externalId, bool isLabeless);
ParameterListASTNode* new_ParameterListASTNode(Array *parameters);
FunctionDeclarationASTNode* new_FunctionDeclarationASTNode(IdentifierASTNode *id, ParameterListASTNode *parameterList, TypeReferenceASTNode *returnType, BlockASTNode *body);
BinaryExpressionASTNode* new_BinaryExpressionASTNode(ExpressionASTNode *left, ExpressionASTNode *right, OperatorType operator);
UnaryExpressionASTNode* new_UnaryExpressionASTNode(ExpressionASTNode *argument, OperatorType operator /*, bool isPrefix*/);
LiteralExpressionASTNode* new_LiteralExpressionASTNode(ValueType type, union TokenValue value);
ArgumentASTNode* new_ArgumentASTNode(ExpressionASTNode *expression, IdentifierASTNode *label);
ArgumentListASTNode* new_ArgumentListASTNode(Array *arguments);
FunctionCallASTNode* new_FunctionCallASTNode(IdentifierASTNode *id, ArgumentListASTNode *argumentList);
PatternASTNode* new_PatternASTNode(IdentifierASTNode *id, TypeReferenceASTNode *type);
OptionalBindingConditionASTNode* new_OptionalBindingConditionASTNode(IdentifierASTNode* id);
IfStatementASTNode* new_IfStatementASTNode(ASTNode *test,  BlockASTNode *body, ASTNode *alternate);
WhileStatementASTNode* new_WhileStatementASTNode(ASTNode *test,  BlockASTNode *body);
AssignmentStatementASTNode* new_AssignmentStatementASTNode(IdentifierASTNode *id, ExpressionASTNode *expression);

// TODO: Add more AST node constructors


/* Other public functions */

ASTNode* ASTNode_alloc(size_t size, enum ASTNodeType type);
void ASTNode_free(ASTNode *node);

void ASTNode_print(ASTNode *node);

#define prepare_node_of(dataType, nodeType) dataType *node = (dataType*)ASTNode_alloc(sizeof(dataType), nodeType);

#endif
