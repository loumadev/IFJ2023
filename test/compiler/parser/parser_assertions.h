#include "compiler/parser/Parser.h"
#include "unit.h"

#ifndef PARSER_ASSERTIONS_H
#define PARSER_ASSERTIONS_H

#define MERGE_(a, b)  a ## b
#define ID(name) "_" ## name ## __LINE__

#define LF "\n"
#define SP "  "
#define TAB "\t"

/**
 * Validates program body and looks for a single statemtnt of a given type.
 * This creates `_program`, `_block`, `_statements` and `statement` variables to use.
 */
#define EXPECT_STATEMENT(node, type) EXPECT_NOT_NULL(node);     \
                                                                \
	ProgramASTNode *_program = (ProgramASTNode*)node;           \
	EXPECT_NOT_NULL(_program->block);                           \
                                                                \
	BlockASTNode *_block = _program->block;                     \
	EXPECT_NOT_NULL(_block->statements);                        \
                                                                \
	Array *_statements = _block->statements;                    \
	EXPECT_EQUAL_INT(_statements->size, 1);                     \
                                                                \
	StatementASTNode *statement = Array_get(_statements, 0);    \
	EXPECT_NOT_NULL(statement);                                 \
	EXPECT_TRUE(statement->_type == type);

/**
 * Validates program body and looks for a statement list of a given size.
 * This creates `_program`, `_block` and `statements` variables to use.
 */
#define EXPECT_STATEMENTS(node, count) EXPECT_NOT_NULL(node);   \
                                                                \
	ProgramASTNode *_program = (ProgramASTNode*)node;           \
	EXPECT_NOT_NULL(_program->block);                           \
                                                                \
	BlockASTNode *_block = _program->block;                     \
	EXPECT_NOT_NULL(_block->statements);                        \
                                                                \
	Array *statements = _block->statements;                     \
	EXPECT_EQUAL_INT(statements->size, count);


#define EXPECT_BINARY_NODE(node, _operator, leftType, rightType, name) EXPECT_NOT_NULL(node);   \
                                                                    \
	BinaryExpressionASTNode *name = (BinaryExpressionASTNode*)node; \
	EXPECT_TRUE(name->_type == NODE_BINARY_EXPRESSION);             \
	EXPECT_TRUE(name->operator == _operator);                       \
                                                                    \
	EXPECT_NOT_NULL(name->left);                                    \
	EXPECT_TRUE(name->left->_type == leftType);                     \
                                                                    \
	EXPECT_NOT_NULL(name->right);                                   \
	EXPECT_TRUE(name->right->_type == rightType);

#endif
