#include <stdio.h>

#include "compiler/parser/ASTNodes.h"
#include "compiler/codegen/Instruction.h"
#include "compiler/codegen/Codegen.h"
#include "internal/String.h"
#include "internal/Array.h"
#include "assertf.h"

void __Codegen_generate(Codegen *codegen);
void __Codegen_generatePreamble();
void __Codegen_generateVariableDeclaration(Codegen *codegen, VariableDeclaration *variable);
void __Codegen_generateGlobalVariablesDeclarations(Codegen *codegen);
// __attribute__((unused)) void __Codegen_generateFunctionsDeclarations(Codegen *codegen);
// void __Codegen_generateFunctionDeclaration(__attribute__((unused)) Codegen *codegen, FunctionDeclaration *function);

// Walking AST functions
void __Codegen_walkAST(Codegen *codegen);
void __Codegen_evaluateIfStatement(Codegen *codegen, IfStatementASTNode *ifStatement);
void __Codegen_evaluateWhileStatement(Codegen *codegen, WhileStatementASTNode *whileStatement);
void __Codegen_evaluateFunctionDeclaration(Codegen *codegen, FunctionDeclarationASTNode *functionDeclaration);
void __Codegen_evaluateBinaryExpression(Codegen *codegen, BinaryExpressionASTNode *binaryExpression);
void __Codegen_evaluateBinaryOperator(__attribute__((unused)) Codegen *codegen, BinaryExpressionASTNode *expression);
void __Codegen_evaluateLiteral(__attribute__((unused)) __attribute__((unused)) Codegen *codegen, LiteralExpressionASTNode *literal);
void __Codegen_evaluateVariableDeclaration(Codegen *codegen, VariableDeclarationASTNode *variableDeclaration);
void __Codegen_evaluateVariableDeclarationList(Codegen *codegen, VariableDeclarationListASTNode *declarationList);
void __Codegen_evaluateVariableDeclarator(Codegen *codegen, VariableDeclaratorASTNode *variableDeclarator);
void __Codegen_evaluateAssignmentStatement(Codegen *codegen, AssignmentStatementASTNode *assignmentStatement);
void __Codegen_evaluateExpressionStatement(Codegen *codegen, ExpressionStatementASTNode *expressionStatement);
void __Codegen_evaluateBlock(__attribute__((unused)) Codegen *codegen, BlockASTNode *block);


void __Codegen_evaluateStatement(Codegen *codegen, StatementASTNode *statementAstNode);

void Codegen_constructor(Codegen *codegen, Analyser *analyser) {
	assertf(codegen != NULL);
	assertf(analyser != NULL);

	codegen->analyser = analyser;
	codegen->frame = FRAME_GLOBAL;
}

void Codegen_destructor(Codegen *codegen) {
	assertf(codegen != NULL);
	assertf(codegen->analyser != NULL);

	codegen->analyser = NULL;
}

void Codegen_generate(Codegen *codegen) {
	assertf(codegen != NULL);
	assertf(codegen->analyser != NULL);

	__Codegen_generate(codegen);
}


void __Codegen_generate(Codegen *codegen) {
	__Codegen_generatePreamble();
	__Codegen_generateGlobalVariablesDeclarations(codegen);
	__Codegen_walkAST(codegen);
}

void __Codegen_generatePreamble() {
	COMMENT("Generated by IFJ2023 compiler")
	COMMENT("Generated with <3")
	HEADER
	NEWLINE
}

void __Codegen_generateGlobalVariablesDeclarations(Codegen *codegen) {
	Array *variables = HashMap_values(codegen->analyser->variables);

	for(size_t i = 0; i < variables->size; i++) {
		VariableDeclaration *declaration = (VariableDeclaration*)Array_get(variables, i);
		__Codegen_generateVariableDeclaration(codegen, declaration);
	}
}

void __Codegen_generateVariableDeclaration(Codegen *codegen, VariableDeclaration *variable) {
	COMMENT_VAR(variable->id)
	Instruction_defvar(variable->id, codegen->frame);
	NEWLINE
}

// __attribute__((unused)) void __Codegen_generateFunctionsDeclarations(Codegen *codegen) {
//	Array *functions = HashMap_values(codegen->analyser->functions);
//	for(size_t i = 0; i < functions->size; i++) {
//		FunctionDeclaration *declaration = (FunctionDeclaration*)Array_get(functions, i);
//		__Codegen_generateFunctionDeclaration(codegen, declaration);
//	}
// }
//
// void __Codegen_generateFunctionDeclaration(__attribute__((unused)) Codegen *codegen, FunctionDeclaration *function) {
//	COMMENT_FUNC(function->id)
//
//	FunctionDeclarationASTNode *node = function->node;
//	Instruction_label_func_start(function->id);
//	Instruction_pushframe();
//
//	if(node->returnType->type.type != TYPE_VOID) {
//		Instruction_defretvar(function->id, FRAME_LOCAL);
//	}
//
// }

void __Codegen_walkAST(Codegen *codegen) {
	BlockASTNode *block = codegen->analyser->ast->block;
	__Codegen_evaluateBlock(codegen, block);
}

void __Codegen_evaluateStatement(Codegen *codegen, StatementASTNode *statementAstNode) {
	switch(statementAstNode->_type) {
		case NODE_IF_STATEMENT: {
			IfStatementASTNode *ifStatement = (IfStatementASTNode*)statementAstNode;
			__Codegen_evaluateIfStatement(codegen, ifStatement);
		} break;
		case NODE_WHILE_STATEMENT: {
			WhileStatementASTNode *whileStatement = (WhileStatementASTNode*)statementAstNode;
			__Codegen_evaluateWhileStatement(codegen, whileStatement);
		} break;
		case NODE_FUNCTION_DECLARATION: {
			FunctionDeclarationASTNode *funcDeclaration = (FunctionDeclarationASTNode*)statementAstNode;
			__Codegen_evaluateFunctionDeclaration(codegen, funcDeclaration);
		} break;
		case NODE_BINARY_EXPRESSION: {
			BinaryExpressionASTNode *binaryExpression = (BinaryExpressionASTNode*)statementAstNode;
			__Codegen_evaluateBinaryExpression(codegen, binaryExpression);
		} break;
		// unwrap - ignore
		case NODE_UNARY_EXPRESSION:
			// TODO: Implement
			break;
		case NODE_LITERAL_EXPRESSION: {
			LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)statementAstNode;
			__Codegen_evaluateLiteral(codegen, literal);
		} break;
		case NODE_FUNCTION_CALL:
			// TODO: Implement
			break;
		case NODE_VARIABLE_DECLARATION: {
			VariableDeclarationASTNode *variableDeclaration = (VariableDeclarationASTNode*)statementAstNode;
			__Codegen_evaluateVariableDeclaration(codegen, variableDeclaration);
		} break;

		// var a = 1, b = 2, c = 3
		case NODE_VARIABLE_DECLARATION_LIST: {
			VariableDeclarationListASTNode *declarationList = (VariableDeclarationListASTNode*)statementAstNode;
			__Codegen_evaluateVariableDeclarationList(codegen, declarationList);
		} break;

		// a = 1, b = 2 (related ku NODE_VARIABLE_DECLARATION_LIST)
		case NODE_VARIABLE_DECLARATOR: {
			VariableDeclaratorASTNode *declaratorNode = (VariableDeclaratorASTNode*)statementAstNode;
			__Codegen_evaluateVariableDeclarator(codegen, declaratorNode);
		} break;

		// a = 1
		case NODE_ASSIGNMENT_STATEMENT: {
			AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statementAstNode;
			__Codegen_evaluateAssignmentStatement(codegen, assignment);
		} break;
		// Len function call
		case NODE_EXPRESSION_STATEMENT: {
			ExpressionStatementASTNode *expressionStatement = (ExpressionStatementASTNode*)statementAstNode;
			__Codegen_evaluateExpressionStatement(codegen, expressionStatement);
		} break;

		case NODE_RETURN_STATEMENT:
			// TODO: Implement
			break;

		case NODE_IDENTIFIER: {
			IdentifierASTNode *identifier = (IdentifierASTNode*)statementAstNode;
			Instruction_pushs_var(identifier->id, codegen->frame);
		} break;

		case NODE_OPTIONAL_BINDING_CONDITION: {
			OptionalBindingConditionASTNode *node = (OptionalBindingConditionASTNode*)statementAstNode;
			Instruction_pushs_var(node->id->id, codegen->frame);
		} break;
		case NODE_BLOCK: {
			BlockASTNode *block = (BlockASTNode*)statementAstNode;
			__Codegen_evaluateBlock(codegen, block);
		} break;
		case NODE_INVALID:
		case NODE_PROGRAM:
		case NODE_TYPE_REFERENCE:
		case NODE_PARAMETER:
		case NODE_PARAMETER_LIST:
		case NODE_ARGUMENT:
		case NODE_ARGUMENT_LIST:
		case NODE_PATTERN:
			fassertf("Unexpected ASTNode type. Analyser probably failed.");
	}
}

void __Codegen_evaluateIfStatement(Codegen *codegen, IfStatementASTNode *ifStatement) {
	COMMENT_IF(ifStatement->id)
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)ifStatement->test);
	Instruction_pushs_bool(true);
	Instruction_jumpifneqs_if_end(ifStatement->id);
	Instruction_clears();

	// Process body
	COMMENT_IF_BLOCK(ifStatement->id)
	__Codegen_evaluateBlock(codegen, ifStatement->body);
	Instruction_clears();

	Instruction_label_if_end(ifStatement->id);
	Instruction_clears();

	// Process else
	if(ifStatement->alternate != NULL) {
		COMMENT_ELSE_BLOCK(ifStatement->id)
		__Codegen_evaluateStatement(codegen, (StatementASTNode*)ifStatement->alternate);
		Instruction_clears();
	}
}

void __Codegen_evaluateWhileStatement(Codegen *codegen, WhileStatementASTNode *whileStatement) {
	COMMENT_WHILE(whileStatement->id)
	// Start label
	Instruction_label_while_start(whileStatement->id);

	// Process test
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)whileStatement->test);
	Instruction_pushs_bool(true);

	// Make actual test
	Instruction_jumpifneqs_while_end(whileStatement->id);
	Instruction_clears();

	// Process body
	__Codegen_evaluateBlock(codegen, whileStatement->body);

	// At end, go to begging for test
	Instruction_jump_while_start(whileStatement->id);

	// End of loop, clear stack
	Instruction_label_while_end(whileStatement->id);
	Instruction_clears();
}

void __Codegen_evaluateFunctionDeclaration(Codegen *codegen, FunctionDeclarationASTNode *functionDeclaration) {
	if(codegen->frame == FRAME_GLOBAL) {
		codegen->frame = FRAME_GLOBAL;
	}

	Instruction_label_func_start(functionDeclaration->id->id);
}

void __Codegen_evaluateBinaryExpression(Codegen *codegen, BinaryExpressionASTNode *binaryExpression) {
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)binaryExpression->left);
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)binaryExpression->right);
	__Codegen_evaluateBinaryOperator(codegen, binaryExpression);
}

// TODO: Careful, there are some exceptions when working with nils
void __Codegen_evaluateBinaryOperator(__attribute__((unused)) Codegen *codegen, BinaryExpressionASTNode *expression) {
	switch(expression->operator) {
		case OPERATOR_PLUS:
			return Instruction_adds();
		case OPERATOR_MINUS:
			return Instruction_subs();
		case OPERATOR_MUL:
			return Instruction_muls();
		case OPERATOR_DIV:
			return Instruction_divs();
		case OPERATOR_EQUAL:
			return Instruction_eqs();
		case OPERATOR_NOT_EQUAL:
			Instruction_eqs();
			Instruction_nots();
			return;
		case OPERATOR_LESS:
			return Instruction_lts();
		case OPERATOR_GREATER:
			return Instruction_gts();
		case OPERATOR_LESS_EQUAL:
			Instruction_gts();
			// Negation of < is =>
			Instruction_nots();
			return;
		case OPERATOR_GREATER_EQUAL:
			Instruction_lts();
			// Negation of > is <=
			Instruction_nots();
			return;
		case OPERATOR_NOT:
			return Instruction_nots();
		case OPERATOR_OR:
			return Instruction_ors();
		case OPERATOR_AND:
			return Instruction_ands();
		case OPERATOR_UNWRAP:
		case OPERATOR_NULL_COALESCING:
			fassertf("Operator not implemented yet. Throw this this against xnovot2r head");
		case OPERATOR_DEFAULT: {
			fassertf("Unknown operator. Analysis probably failed.");
		}
	}
}

void __Codegen_evaluateLiteral(__attribute__((unused)) Codegen *codegen, LiteralExpressionASTNode *literal) {
	switch(literal->type.type) {
		case TYPE_NIL:
			return Instruction_pushs_nil();
		case TYPE_INT:
			return Instruction_pushs_int(literal->value.integer);
		case TYPE_DOUBLE:
			return Instruction_pushs_float(literal->value.floating);
		case TYPE_BOOL:
			return Instruction_pushs_bool(literal->value.boolean);
		case TYPE_STRING:
			return Instruction_pushs_string(literal->value.string);
		case TYPE_VOID:
			// TODO: Careful, you need to think this through
			return;
		case TYPE_UNKNOWN:
		case TYPE_INVALID:
			fassertf("Unknown or invalid type. Analysis probably failed.");
	}
}

void __Codegen_evaluateVariableDeclaration(Codegen *codegen, VariableDeclarationASTNode *variableDeclaration) {
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)variableDeclaration->declaratorList);
}

void __Codegen_evaluateVariableDeclarationList(Codegen *codegen, VariableDeclarationListASTNode *declarationList) {
	Array *declarators = declarationList->declarators;
	for(size_t i = 0; i < declarators->size; ++i) {
		VariableDeclaratorASTNode *declaratorNode = Array_get(declarators, i);
		__Codegen_evaluateStatement(codegen, (StatementASTNode*)declaratorNode);
	}
}

void __Codegen_evaluateVariableDeclarator(Codegen *codegen, VariableDeclaratorASTNode *variableDeclarator) {
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)variableDeclarator->initializer);
	Instruction_pops(variableDeclarator->pattern->id->id, codegen->frame);
	NEWLINE
}

void __Codegen_evaluateAssignmentStatement(Codegen *codegen, AssignmentStatementASTNode *assignmentStatement) {
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)assignmentStatement->expression);
	Instruction_pops(assignmentStatement->id->id, codegen->frame);
	NEWLINE
}

void __Codegen_evaluateExpressionStatement(Codegen *codegen, ExpressionStatementASTNode *expressionStatement) {
	__Codegen_evaluateStatement(codegen, (StatementASTNode*)expressionStatement->expression);
	Instruction_clears();
}

void __Codegen_evaluateBlock(Codegen *codegen, BlockASTNode *block) {
	Array *statements = block->statements;

	for(size_t i = 0; i < statements->size; ++i) {
		StatementASTNode *statement = Array_get(statements, i);
		__Codegen_evaluateStatement(codegen, statement);
	}
}