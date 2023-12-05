/**
 * @file include/compiler/parser/ASTNodes.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @author Veronika Krobotová <xkrobo03@stud.fit.vut.cz>
 * @author Radim Mifka <xmifka00@stud.fit.vut.cz>
 * @brief Header file for abstract syntax tree nodes.
 * @copyright Copyright (c) 2023
 */

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
	NODE_BREAK_STATEMENT,
	NODE_CONTINUE_STATEMENT,
	NODE_PARAMETER,
	NODE_PARAMETER_LIST,
	NODE_FUNCTION_DECLARATION,
	NODE_ARGUMENT,
	NODE_BINARY_EXPRESSION,
	NODE_UNARY_EXPRESSION,
	NODE_LITERAL_EXPRESSION,
	NODE_INTERPOLATION_EXPRESSION,
	NODE_ARGUMENT_LIST,
	NODE_FUNCTION_CALL,
	NODE_IF_STATEMENT,
	NODE_PATTERN,
	NODE_OPTIONAL_BINDING_CONDITION,
	NODE_RANGE,
	NODE_WHILE_STATEMENT,
	NODE_FOR_STATEMENT,
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
	OPERATOR_GREATER_EQUAL,
	OPERATOR_NOT,
	OPERATOR_OR,
	OPERATOR_AND,
	OPERATOR_RANGE,
	OPERATOR_HALF_OPEN_RANGE
} OperatorType;

enum BuiltInType {
	TYPE_NIL = -2,
	TYPE_UNKNOWN = -1,
	TYPE_INVALID = 0,
	TYPE_INT,
	TYPE_DOUBLE,
	TYPE_BOOL,
	TYPE_STRING,
	TYPE_VOID
};

enum BuiltInFunction {
	FUNCTION_NONE = -1,
	FUNCTION_READ_STRING = 0,
	FUNCTION_READ_INT,
	FUNCTION_READ_DOUBLE,
	FUNCTION_WRITE,
	FUNCTION_INT_TO_DOUBLE,
	FUNCTION_DOUBLE_TO_INT,
	FUNCTION_LENGTH,
	FUNCTION_SUBSTRING,
	FUNCTION_ORD,
	FUNCTION_CHR,
	FUNCTION_INTERNAL_STRINGIFY_NUMBER,
	FUNCTION_INTERNAL_STRINGIFY_DOUBLE,
	FUNCTION_INTERNAL_STRINGIFY_INT,
	FUNCTION_INTERNAL_STRINGIFY_BOOL,
	FUNCTION_INTERNAL_STRINGIFY_STRING,
	FUNCTION_INTERNAL_MODULO,
	FUNCTIONS_COUNT
};

#define is_func_valid(func) ((func) < FUNCTIONS_COUNT)
#define is_func_user_defined(func) ((func) == FUNCTION_NONE)
#define is_func_builtin(func) ((func) > FUNCTION_NONE && (func) < FUNCTION_INTERNAL_STRINGIFY_NUMBER)
#define is_func_internal(func) ((func) >= FUNCTION_INTERNAL_STRINGIFY_NUMBER && (func) < FUNCTIONS_COUNT)
#define is_func_generable(func) (is_func_user_defined(func) || is_func_internal(func))

#define is_type_valid(type) ((type) > TYPE_INVALID)
#define is_value_assignable(dst, src) (((dst).type == (src).type || (src).type == TYPE_NIL) && ((dst).isNullable || !(src).isNullable))
#define is_type_equal(type1, type2) ((type1).type == (type2).type && (type1).isNullable == (type2).isNullable)

typedef struct ValueType {
	enum BuiltInType type;
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
	ExpressionASTNode /* | null*/ *initializer;
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
	ExpressionASTNode /* | null*/ *expression;
	size_t id;
} ReturnStatementASTNode;

typedef struct BreakStatementASTNode {
	enum ASTNodeType _type;
	size_t id; // This must be the second field
} BreakStatementASTNode;

typedef struct ContinueStatementASTNode {
	enum ASTNodeType _type;
	size_t id; // This must be the second field
} ContinueStatementASTNode;

typedef struct ParameterASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *internalId;
	TypeReferenceASTNode /* | null*/ *type;
	ExpressionASTNode /* | null*/ *initializer;
	IdentifierASTNode /* | null*/ *externalId; // If null then internalId should be used
	bool isLabeless; // true if externalId is '_'
} ParameterASTNode;

typedef struct ParameterListASTNode {
	enum ASTNodeType _type;
	Array /*<ParameterASTNode>*/ *parameters;
} ParameterListASTNode;

typedef struct FunctionDeclarationASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	ParameterListASTNode *parameterList;
	TypeReferenceASTNode /* | null*/ *returnType; // For fully resolved return type (e.g. Void) query analyser for declaration by id
	BlockASTNode *body;
	enum BuiltInFunction builtin; // FUNCTION_NONE identifies user defined function
} FunctionDeclarationASTNode;

typedef struct ArgumentASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *expression;
	IdentifierASTNode /* | null*/ *label;
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
	bool isPrefix;
	struct ValueType type;
} UnaryExpressionASTNode;

typedef struct LiteralExpressionASTNode {
	enum ASTNodeType _type;
	union TokenValue value;
	struct ValueType type;
	union TokenValue originalValue;
	struct ValueType originalType;
} LiteralExpressionASTNode;

typedef struct InterpolationExpressionASTNode {
	enum ASTNodeType _type;
	Array /*<String>*/ *strings;
	Array /*<ExpressionASTNode>*/ *expressions; // Always has one less element than strings
	BinaryExpressionASTNode *concatenated;
} InterpolationExpressionASTNode;

typedef struct PatternASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	TypeReferenceASTNode /* | null*/ *type;
} PatternASTNode;

typedef struct OptionalBindingConditionASTNode {
	enum ASTNodeType _type;
	IdentifierASTNode *id;
	size_t fromId;
} OptionalBindingConditionASTNode;

typedef struct IfStatementASTNode {
	enum ASTNodeType _type;
	BlockASTNode *body; // This must be the second field
	ASTNode /*<ExpressionASTNode | OptionalBindingConditionASTNode>*/ *test; // This must be the third field
	ASTNode /*<BlockASTNode | IfStatementASTNode> | null*/ *alternate;
	size_t id;
} IfStatementASTNode;

typedef struct WhileStatementASTNode {
	enum ASTNodeType _type;
	BlockASTNode *body; // This must be the second field
	ASTNode /*<ExpressionASTNode | OptionalBindingConditionASTNode>*/ *test; // This must be the third field
	size_t id; // This must be the fourth field
} WhileStatementASTNode;

typedef struct RangeASTNode {
	enum ASTNodeType _type;
	ExpressionASTNode *start;
	ExpressionASTNode *end;
	size_t endId;
	enum OperatorType operator;
} RangeASTNode;

typedef struct ForStatementASTNode {
	enum ASTNodeType _type;
	BlockASTNode *body; // This must be the second field
	IdentifierASTNode *iterator;
	size_t id; // This must be the fourth field
	RangeASTNode *range;
} ForStatementASTNode;

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
BreakStatementASTNode* new_BreakStatementASTNode();
ContinueStatementASTNode* new_ContinueStatementASTNode();
ParameterASTNode* new_ParameterASTNode(IdentifierASTNode *internalId, TypeReferenceASTNode *type, ExpressionASTNode *initializer, IdentifierASTNode *externalId, bool isLabeless);
ParameterListASTNode* new_ParameterListASTNode(Array *parameters);
FunctionDeclarationASTNode* new_FunctionDeclarationASTNode(IdentifierASTNode *id, ParameterListASTNode *parameterList, TypeReferenceASTNode *returnType, BlockASTNode *body);
BinaryExpressionASTNode* new_BinaryExpressionASTNode(ExpressionASTNode *left, ExpressionASTNode *right, OperatorType operator);
UnaryExpressionASTNode* new_UnaryExpressionASTNode(ExpressionASTNode *argument, OperatorType operator, bool isPrefix);
LiteralExpressionASTNode* new_LiteralExpressionASTNode(ValueType type, union TokenValue value);
InterpolationExpressionASTNode* new_InterpolationExpressionASTNode(Array /*<String>*/ *strings, Array /*<ExpressionASTNode>*/ *expressions);
ArgumentASTNode* new_ArgumentASTNode(ExpressionASTNode *expression, IdentifierASTNode *label);
ArgumentListASTNode* new_ArgumentListASTNode(Array *arguments);
FunctionCallASTNode* new_FunctionCallASTNode(IdentifierASTNode *id, ArgumentListASTNode *argumentList);
PatternASTNode* new_PatternASTNode(IdentifierASTNode *id, TypeReferenceASTNode *type);
OptionalBindingConditionASTNode* new_OptionalBindingConditionASTNode(IdentifierASTNode *id);
IfStatementASTNode* new_IfStatementASTNode(ASTNode *test,  BlockASTNode *body, ASTNode *alternate);
WhileStatementASTNode* new_WhileStatementASTNode(ASTNode *test,  BlockASTNode *body);
ForStatementASTNode* new_ForStatementASTNode(IdentifierASTNode *iterator, RangeASTNode *range, BlockASTNode *body);
RangeASTNode* new_RangeASTNode(ExpressionASTNode *start, ExpressionASTNode *end, OperatorType operator);
AssignmentStatementASTNode* new_AssignmentStatementASTNode(IdentifierASTNode *id, ExpressionASTNode *expression);
ExpressionStatementASTNode* new_ExpressionStatementASTNode(ExpressionASTNode *expression);

// TODO: Add more AST node constructors


/* Other public functions */

ASTNode* ASTNode_alloc(size_t size, enum ASTNodeType type);
void ASTNode_free(ASTNode *node);

void ASTNode_print(ASTNode *node);

#define prepare_node_of(dataType, nodeType) dataType *node = (dataType*)ASTNode_alloc(sizeof(dataType), nodeType);

#endif

/** End of file include/compiler/parser/ASTNodes.h **/
