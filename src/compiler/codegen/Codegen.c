/**
 * @file src/compiler/codegen/Codegen.c
 * @author Jaroslav Novotny <xnovot2r@stud.fit.vutbr.cz>
 * @brief Implementation of code generation.
 * @copyright Copyright (c) 2023
 */

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
void __Codegen_generateBuiltInFunctions();
void __Codegen_generateUserFunctions(Codegen *codegen);
void __Codegen_generateGlobalVariablesDeclarations(Codegen *codegen);
void __Codegen_generateHelperVariables();

// Built-in functions
void __Codegen_generateOrd();
void __Codegen_generateChr();
void __Codegen_generateLength();
void __Codegen_generateSubstring();

// User functions
void __Codegen_generateFunctionDeclaration(Codegen *codegen, FunctionDeclaration *functionDeclaration);

// Walking AST functions
void __Codegen_generateMain(Codegen *codegen);
void __Codegen_evaluateIfStatement(Codegen *codegen, IfStatementASTNode *ifStatement);
void __Codegen_evaluateWhileStatement(Codegen *codegen, WhileStatementASTNode *whileStatement);
void __Codegen_evaluateBinaryExpression(Codegen *codegen, BinaryExpressionASTNode *binaryExpression);
void __Codegen_evaluateBinaryOperator(BinaryExpressionASTNode *expression);
void __Codegen_evaluateLiteral(LiteralExpressionASTNode *literal);
void __Codegen_evaluateVariableDeclaration(Codegen *codegen, VariableDeclarationASTNode *variableDeclaration);
void __Codegen_evaluateVariableDeclarationList(Codegen *codegen, VariableDeclarationListASTNode *declarationList);
void __Codegen_evaluateVariableDeclarator(Codegen *codegen, VariableDeclaratorASTNode *variableDeclarator);
void __Codegen_evaluateAssignmentStatement(Codegen *codegen, AssignmentStatementASTNode *assignmentStatement);
void __Codegen_evaluateExpressionStatement(Codegen *codegen, ExpressionStatementASTNode *expressionStatement);
void __Codegen_evaluateExpression(Codegen *codegen, ExpressionASTNode *expression);
void __Codegen_evaluateBlock(Codegen *codegen, BlockASTNode *block);
void __Codegen_evaluateFunctionCall(Codegen *codegen, FunctionCallASTNode *functionCall);
void __Codegen_evaluateStatement(Codegen *codegen, StatementASTNode *statementAstNode);
void __Codegen_evaluateReturnStatement(Codegen *codegen, ReturnStatementASTNode *returnStatement);
void __Codegen_evaluateBindingCondition(Codegen *codegen, OptionalBindingConditionASTNode *optionalBindingCondition);
// void __Codegen_evaluateForStatement(Codegen *codegen, ForStatementASTNode *forStatement);

// Optimalizations
void __Codegen_generateCoalescing();
void __Codegen_resolveBuiltInFunction(Codegen *codegen, FunctionCallASTNode *functionCall, enum BuiltInFunction function);
void __Codegen_stackAssignment(Codegen *codegen, AssignmentStatementASTNode *assignmentStatement, bool isGlobalVariable);
void __Codegen_directAssignment(const Codegen *codegen, const AssignmentStatementASTNode *assignmentStatement, bool isGlobalVariable);

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
	__Codegen_generateHelperVariables();

	// Jump to main
	Instruction_jump("main");
	NEWLINE

	__Codegen_generateBuiltInFunctions();
	__Codegen_generateUserFunctions(codegen);

	// Main
	COMMENT("--- [Main] ---")
	Instruction_label("main");
	NEWLINE

	__Codegen_generateGlobalVariablesDeclarations(codegen);
	__Codegen_generateMain(codegen);
}

void __Codegen_generatePreamble() {
	COMMENT("Generated by IFJ2023 compiler")
	COMMENT("Generated with <3")
	HEADER
	NEWLINE
}

void __Codegen_generateHelperVariables() {
	COMMENT("--- [Helper variables] ---")

	Instruction_defvar_where("WRITE_TMP", FRAME_GLOBAL);
	Instruction_defvar_where("READINT_TMP", FRAME_GLOBAL);
	Instruction_defvar_where("READSTRING_TMP", FRAME_GLOBAL);
	Instruction_defvar_where("READDOUBLE_TMP", FRAME_GLOBAL);
	Instruction_defvar_where("CONCAT_ARG1", FRAME_GLOBAL);
	Instruction_defvar_where("CONCAT_ARG2", FRAME_GLOBAL);
	Instruction_defvar_where("CONCAT_OUTPUT", FRAME_GLOBAL);
	NEWLINE
}

void __Codegen_generateBuiltInFunctions() {
	COMMENT("--- [Built-in functions] ---")

	__Codegen_generateOrd();
	__Codegen_generateChr();
	__Codegen_generateLength();
	__Codegen_generateSubstring();
	__Codegen_generateCoalescing();
}

void __Codegen_generateOrd() {
	COMMENT("[Builtin] ord(string)")
	Instruction_label("ord");

	// Overhead
	Instruction_pushframe();
	Instruction_defvar_where("RETVAL_ORD", FRAME_LOCAL);

	// Overhead for calling length
	Instruction_createframe();
	Instruction_defvar_where("ARG1_LEN", FRAME_TEMPORARY);
	Instruction_move_vars(FRAME_TEMPORARY, "ARG1_LEN", FRAME_LOCAL, "ARG1_ORD");

	// Call length
	Instruction_call("length");

	// Handle length(...) return value
	Instruction_defvar_where("STRLEN_OUTPUT", FRAME_LOCAL);
	Instruction_move_vars(FRAME_LOCAL, "STRLEN_OUTPUT", FRAME_TEMPORARY, "RETVAL_LEN");

	// Return value can be implicitly zero
	Instruction_move_int(FRAME_LOCAL, "RETVAL_ORD", 0);

	// Check if length is 0
	Instruction_pushs_var_named("STRLEN_OUTPUT", FRAME_LOCAL);
	Instruction_pushs_int(0);
	Instruction_jump_ifeqs("ord_check_length");

	// Not 0, get ord at index 0
	Instruction_stri2int(FRAME_LOCAL, "RETVAL_ORD", FRAME_LOCAL, "ARG1_ORD", 0);

	Instruction_label("ord_check_length");

	// Handle return value
	Instruction_popframe();
	Instruction_return();

	NEWLINE
}

void __Codegen_generateLength() {
	COMMENT("[Builtin] length(string)")

	// Overhead
	Instruction_label("length");
	Instruction_pushframe();

	// Handle return value
	Instruction_defvar_where("RETVAL_LEN", FRAME_LOCAL);

	// Length implementation
	Instruction_strlen(FRAME_LOCAL, "RETVAL_LEN", "ARG1_LEN", FRAME_LOCAL);

	Instruction_popframe();
	Instruction_return();

	NEWLINE
}

void __Codegen_generateChr() {
	COMMENT("[Builtin] chr(string)")

	// Overhead
	Instruction_label("chr");
	Instruction_pushframe();

	// Handle return value
	Instruction_defvar_where("RETVAL_CHR", FRAME_LOCAL);

	// chr implementation
	Instruction_int2char(FRAME_LOCAL, "RETVAL_CHR", FRAME_LOCAL, "ARG1_CHR");

	// Handle return value
	Instruction_popframe();
	Instruction_return();

	NEWLINE
}

void __Codegen_generateSubstring() {
	COMMENT("[Builtin] substr(string, int, int)")
	Instruction_label("substr");

	// Overhead
	Instruction_pushframe();
	Instruction_defvar_where("RETVAL_SUBSTR", FRAME_LOCAL);

	// Substr is implicitly nil
	Instruction_move_nil(FRAME_LOCAL, "RETVAL_SUBSTR");

	// Substr checks

	// i < 0
	Instruction_pushs_var_named("ARG2_SUBSTR", FRAME_LOCAL);
	Instruction_pushs_int(0);
	Instruction_lts();
	Instruction_pushs_bool(true);
	Instruction_jump_ifeqs("substr_end");

	// j < 0
	Instruction_pushs_var_named("ARG3_SUBSTR", FRAME_LOCAL);
	Instruction_pushs_int(0);
	Instruction_lts();
	Instruction_pushs_bool(true);
	Instruction_jump_ifeqs("substr_end");

	// i > j
	Instruction_pushs_var_named("ARG2_SUBSTR", FRAME_LOCAL);
	Instruction_pushs_var_named("ARG3_SUBSTR", FRAME_LOCAL);
	Instruction_gts();
	Instruction_pushs_bool(true);
	Instruction_jump_ifeqs("substr_end");

	// i >= length(string) and j > length(string)

	// Overhead for calling length
	Instruction_createframe();
	Instruction_defvar_where("ARG1_LEN", FRAME_TEMPORARY);
	Instruction_move_vars(FRAME_TEMPORARY, "ARG1_LEN", FRAME_LOCAL, "ARG1_SUBSTR");

	// Call length
	Instruction_call("length");

	// Handle return value
	Instruction_defvar_where("STRLEN_OUTPUT", FRAME_LOCAL);
	Instruction_move_vars(FRAME_LOCAL, "STRLEN_OUTPUT", FRAME_TEMPORARY, "RETVAL_LEN");

	// Check if i >= length(string)
	Instruction_pushs_var_named("ARG2_SUBSTR", FRAME_LOCAL);
	Instruction_pushs_var_named("STRLEN_OUTPUT", FRAME_LOCAL);
	Instruction_lts();
	Instruction_nots();
	Instruction_pushs_bool(true);
	Instruction_jump_ifeqs("substr_end");

	// Check if j > length(string)
	Instruction_pushs_var_named("ARG3_SUBSTR", FRAME_LOCAL);
	Instruction_pushs_var_named("STRLEN_OUTPUT", FRAME_LOCAL);
	Instruction_gts();
	Instruction_pushs_bool(true);
	Instruction_jump_ifeqs("substr_end");

	// All checks passed, get substr
	Instruction_defvar_where("SUBSTR_BUFFER", FRAME_LOCAL);

	String string;
	String_constructor(&string, "");
	Instruction_move_string(FRAME_LOCAL, "SUBSTR_BUFFER", &string);
	Instruction_move_vars(FRAME_LOCAL, "RETVAL_SUBSTR", FRAME_LOCAL, "SUBSTR_BUFFER");

	Instruction_defvar_where("SUBSTR_GETCHAR", FRAME_LOCAL);

	Instruction_label("substr_loop");

	// Check if i < j
	Instruction_pushs_var_named("ARG2_SUBSTR", FRAME_LOCAL);
	Instruction_pushs_var_named("ARG3_SUBSTR", FRAME_LOCAL);
	Instruction_lts();
	Instruction_pushs_bool(false);
	Instruction_jump_ifeqs("substr_end");

	Instruction_getchar(FRAME_LOCAL, "SUBSTR_GETCHAR", FRAME_LOCAL, "ARG1_SUBSTR", FRAME_LOCAL, "ARG2_SUBSTR");
	Instruction_concat(FRAME_LOCAL, "SUBSTR_BUFFER", FRAME_LOCAL, "SUBSTR_BUFFER", FRAME_LOCAL, "SUBSTR_GETCHAR");

	// Increment i
	Instruction_add_int(FRAME_LOCAL, "ARG2_SUBSTR", FRAME_LOCAL, "ARG2_SUBSTR", 1);

	// Handle generated string
	Instruction_move_vars(FRAME_LOCAL, "RETVAL_SUBSTR", FRAME_LOCAL, "SUBSTR_BUFFER");
	Instruction_jump("substr_loop");

	Instruction_label("substr_end");
	Instruction_popframe();
	Instruction_return();

	NEWLINE
}

void __Codegen_generateCoalescing() {
	COMMENT("[Builtin] coalescing(a, b)")
	Instruction_label("coalescing");

	// Overhead
	Instruction_pushframe();
	Instruction_defvar_where("RETVAL_COA", FRAME_LOCAL);

	// Return value is implicitly right (test is nil)
	Instruction_move_vars(FRAME_LOCAL, "RETVAL_COA", FRAME_LOCAL, "ARG_RIGHT_COA");

	// Do test
	Instruction_pushs_var_named("ARG_LEFT_COA", FRAME_LOCAL);
	Instruction_pushs_nil();
	Instruction_jump_ifeqs("coalescing_return");

	// If test is not nil, return left
	Instruction_move_vars(FRAME_LOCAL, "RETVAL_COA", FRAME_LOCAL, "ARG_LEFT_COA");

	Instruction_label("coalescing_return");
	Instruction_popframe();
	Instruction_return();

	NEWLINE
}

void __Codegen_generateGlobalVariablesDeclarations(Codegen *codegen) {
	Array *variables = HashMap_values(codegen->analyser->variables);

	for(size_t i = 0; i < variables->size; i++) {
		VariableDeclaration *declaration = (VariableDeclaration*)Array_get(variables, i);
		__Codegen_generateVariableDeclaration(codegen, declaration);
	}
}

void __Codegen_generateUserFunctions(Codegen *codegen) {
	COMMENT("--- [User-defined functions] ---")

	Array *functions = HashMap_values(codegen->analyser->functions);

	for(size_t i = 0; i < functions->size; i++) {
		FunctionDeclaration *function = (FunctionDeclaration*)Array_get(functions, i);
		__Codegen_generateFunctionDeclaration(codegen, function);
	}
}

void __Codegen_generateFunctionDeclaration(Codegen *codegen, FunctionDeclaration *functionDeclaration) {
	if(!is_func_generable(functionDeclaration->node->builtin)) {
		return;
	}

	COMMENT_FUNC(functionDeclaration->node)
	codegen->frame = FRAME_LOCAL;
	Instruction_label_func(functionDeclaration->id);

	// Overhead
	Instruction_pushframe();

	Array *variables = HashMap_values(functionDeclaration->variables);
	for(size_t i = 0; i < variables->size; i++) {
		VariableDeclaration *declaration = (VariableDeclaration*)Array_get(variables, i);
		__Codegen_generateVariableDeclaration(codegen, declaration);
	}

	// Process body
	__Codegen_evaluateBlock(codegen, functionDeclaration->node->body);

	// Implicit return
	Instruction_return();
	codegen->frame = FRAME_GLOBAL;
	NEWLINE
}

void __Codegen_generateVariableDeclaration(Codegen *codegen, VariableDeclaration *variable) {
	COMMENT_VAR(variable)
	Instruction_defvar(variable->id, codegen->frame);
	NEWLINE
}

void __Codegen_generateMain(Codegen *codegen) {
	BlockASTNode *block = codegen->analyser->ast->block;
	__Codegen_evaluateBlock(codegen, block);
}

void __Codegen_evaluateReturnStatement(Codegen *codegen, ReturnStatementASTNode *returnStatement) {
	FunctionDeclaration *functionDeclaration = Analyser_getFunctionById(codegen->analyser, returnStatement->id);

	if(returnStatement->expression != NULL) {
		__Codegen_evaluateExpression(codegen, returnStatement->expression);

		if(functionDeclaration->returnType.type != TYPE_VOID) {
			Instruction_popretvar(returnStatement->id, codegen->frame);
		}
	}

	Instruction_popframe();
	Instruction_return();
}

void __Codegen_evaluateStatement(Codegen *codegen, StatementASTNode *statementAstNode) {
	switch(statementAstNode->_type) {
		case NODE_VARIABLE_DECLARATION: {
			VariableDeclarationASTNode *variableDeclaration = (VariableDeclarationASTNode*)statementAstNode;
			__Codegen_evaluateVariableDeclaration(codegen, variableDeclaration);
		} break;
		case NODE_ASSIGNMENT_STATEMENT: {
			AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statementAstNode;
			__Codegen_evaluateAssignmentStatement(codegen, assignment);
		} break;
		case NODE_IF_STATEMENT: {
			IfStatementASTNode *ifStatement = (IfStatementASTNode*)statementAstNode;
			__Codegen_evaluateIfStatement(codegen, ifStatement);
		} break;
		case NODE_WHILE_STATEMENT: {
			WhileStatementASTNode *whileStatement = (WhileStatementASTNode*)statementAstNode;
			__Codegen_evaluateWhileStatement(codegen, whileStatement);
		} break;
		case NODE_RETURN_STATEMENT: {
			ReturnStatementASTNode *returnStatement = (ReturnStatementASTNode*)statementAstNode;
			__Codegen_evaluateReturnStatement(codegen, returnStatement);

		} break;
		case NODE_EXPRESSION_STATEMENT: {
			ExpressionStatementASTNode *expressionStatement = (ExpressionStatementASTNode*)statementAstNode;
			__Codegen_evaluateExpressionStatement(codegen, expressionStatement);
		} break;
		case NODE_FOR_STATEMENT: {
//			ForStatementASTNode *forStatement = (ForStatementASTNode*)statementAstNode;
//			__Codegen_evaluateForStatement(codegen, forStatement);
			fassertf("[Codegen] For statement not implemented.");
		} break;
		case NODE_BREAK_STATEMENT: {
			fassertf("[Codegen] Break statement outside of loop.");
//            BreakStatementASTNode *breakStatement = (BreakStatementASTNode*)statementAstNode;
//            __Codegen_evaluateBreakStatement(codegen, breakStatement);
		} break;
		case NODE_CONTINUE_STATEMENT: {
			fassertf("[Codegen] Continue statement outside of loop.");
//            ContinueStatementASTNode *continueStatement = (ContinueStatementASTNode*)statementAstNode;
//            __Codegen_evaluateContinueStatement(codegen, continueStatement);
		} break;
		case NODE_FUNCTION_DECLARATION: {
			// Declarations are generated at the beginning of the file
		} break;
		default:
			fassertf("[Codegen] Unexpected ASTNode type (evaluateStatement).");
	}
}

void __Codegen_evaluateBindingCondition(Codegen *codegen, OptionalBindingConditionASTNode *optionalBindingCondition) {
	Instruction_defvar(optionalBindingCondition->id->id, codegen->frame);
	Instruction_move_id(codegen->frame, optionalBindingCondition->id->id, FRAME_GLOBAL, optionalBindingCondition->fromId);
	Instruction_pushs_var(optionalBindingCondition->fromId, codegen->frame);
	Instruction_pushs_nil();
	Instruction_eqs();
	Instruction_nots();
}

void __Codegen_evaluateIfStatement(Codegen *codegen, IfStatementASTNode *ifStatement) {
	COMMENT_IF(ifStatement->id)
	if(ifStatement->test->_type == NODE_OPTIONAL_BINDING_CONDITION) {
		OptionalBindingConditionASTNode *optionalBindingCondition = (OptionalBindingConditionASTNode*)ifStatement->test;
		__Codegen_evaluateBindingCondition(codegen, optionalBindingCondition);
	} else {
		__Codegen_evaluateExpression(codegen, ifStatement->test);
	}

	Instruction_pushs_bool(true);
	Instruction_jumpifneqs_if_else(ifStatement->id);

	// Process body
	COMMENT_IF_BLOCK(ifStatement->id)
	__Codegen_evaluateBlock(codegen, ifStatement->body);
	Instruction_jump_if_end(ifStatement->id);

	// Process else
	Instruction_label_if_else(ifStatement->id);
	if(ifStatement->alternate != NULL) {
		COMMENT_ELSE_BLOCK(ifStatement->id)
		switch(ifStatement->alternate->_type) {
			case NODE_BLOCK:
				__Codegen_evaluateBlock(codegen, (BlockASTNode*)ifStatement->alternate);
				break;
			case NODE_IF_STATEMENT:
				__Codegen_evaluateIfStatement(codegen, (IfStatementASTNode*)ifStatement->alternate);
				break;
			default:
				fassertf("[Codegen] Unexpected ASTNode type (evaluateIfStatement).");
		}

	}

	Instruction_label_if_end(ifStatement->id);
}

void __Codegen_evaluateWhileStatement(Codegen *codegen, WhileStatementASTNode *whileStatement) {
	COMMENT_WHILE(whileStatement->id)
	// Start label
	Instruction_label_while_start(whileStatement->id);

	// Process test
	if(whileStatement->test->_type == NODE_OPTIONAL_BINDING_CONDITION) {
		OptionalBindingConditionASTNode *optionalBindingCondition = (OptionalBindingConditionASTNode*)whileStatement->test;
		__Codegen_evaluateBindingCondition(codegen, optionalBindingCondition);
	} else {
		__Codegen_evaluateExpression(codegen, whileStatement->test);
	}
	Instruction_pushs_bool(true);

	// Make actual test
	Instruction_jumpifneqs_while_end(whileStatement->id);

	// Process body
	__Codegen_evaluateBlock(codegen, whileStatement->body);

	// At end, go to begging for test
	Instruction_jump_while_start(whileStatement->id);

	// End of loop, clear stack
	Instruction_label_while_end(whileStatement->id);
}

void __Codegen_evaluateBinaryExpression(Codegen *codegen, BinaryExpressionASTNode *binaryExpression) {
	__Codegen_evaluateExpression(codegen, binaryExpression->left);
	__Codegen_evaluateExpression(codegen, binaryExpression->right);
	__Codegen_evaluateBinaryOperator(binaryExpression);
}

// TODO: Careful, there are some exceptions when working with nils
void __Codegen_evaluateBinaryOperator(BinaryExpressionASTNode *expression) {
	assertf(expression != NULL);

	switch(expression->operator) {
		case OPERATOR_PLUS: {
			if(expression->type.type == TYPE_STRING) {
				Instruction_pops_where("CONCAT_ARG2", FRAME_GLOBAL);
				Instruction_pops_where("CONCAT_ARG1", FRAME_GLOBAL);
				Instruction_concat(FRAME_GLOBAL, "CONCAT_OUTPUT", FRAME_GLOBAL, "CONCAT_ARG1", FRAME_GLOBAL, "CONCAT_ARG2");
				Instruction_pushs_var_named("CONCAT_OUTPUT", FRAME_GLOBAL);
				return;
			}
			return Instruction_adds();
		}
		case OPERATOR_MINUS:
			return Instruction_subs();
		case OPERATOR_MUL:
			return Instruction_muls();
		case OPERATOR_DIV:
			if(expression->type.type == TYPE_INT) {
				return Instruction_idivs();
			} else {
				return Instruction_divs();
			}
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
		case OPERATOR_NULL_COALESCING: {
			Instruction_createframe();
			Instruction_defvar_where("ARG_RIGHT_COA", FRAME_TEMPORARY);
			Instruction_pops_where("ARG_RIGHT_COA", FRAME_TEMPORARY);

			Instruction_defvar_where("ARG_LEFT_COA", FRAME_TEMPORARY);
			Instruction_pops_where("ARG_LEFT_COA", FRAME_TEMPORARY);

			Instruction_call("coalescing");
			Instruction_pushs_var_named("RETVAL_COA", FRAME_TEMPORARY);
			return;
		}
		case OPERATOR_UNWRAP:
		case OPERATOR_DEFAULT:
		case OPERATOR_RANGE:
		case OPERATOR_HALF_OPEN_RANGE: {
			fassertf("[Codegen] Unknown operator.");
		}
	}
}

void __Codegen_evaluateLiteral(LiteralExpressionASTNode *literal) {
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
		// void is *practically* not a literal
		case TYPE_UNKNOWN:
		case TYPE_INVALID:
			fassertf("[Codegen] Unknown or invalid literal type.");
	}
}

void __Codegen_evaluateVariableDeclaration(Codegen *codegen, VariableDeclarationASTNode *variableDeclaration) {
	__Codegen_evaluateVariableDeclarationList(codegen, variableDeclaration->declaratorList);
}

void __Codegen_evaluateVariableDeclarationList(Codegen *codegen, VariableDeclarationListASTNode *declarationList) {
	Array *declarators = declarationList->declarators;
	for(size_t i = 0; i < declarators->size; ++i) {
		VariableDeclaratorASTNode *declaratorNode = Array_get(declarators, i); \
		__Codegen_evaluateVariableDeclarator(codegen, declaratorNode);
	}
}

void __Codegen_evaluateVariableDeclarator(Codegen *codegen, VariableDeclaratorASTNode *variableDeclarator) {
	if(variableDeclarator->initializer == NULL) {
		return;
	}
	__Codegen_evaluateExpression(codegen, variableDeclarator->initializer);
	if(Analyser_isDeclarationGlobal(codegen->analyser, variableDeclarator->pattern->id->id)) {
		Instruction_pops(variableDeclarator->pattern->id->id, FRAME_GLOBAL);
	} else {
//		Instruction_defvar(variableDeclarator->pattern->id->id, codegen->frame);
		Instruction_pops(variableDeclarator->pattern->id->id, codegen->frame);
	}
	NEWLINE
}

void __Codegen_stackAssignment(Codegen *codegen, AssignmentStatementASTNode *assignmentStatement, bool isGlobalVariable) {
	__Codegen_evaluateExpression(codegen, assignmentStatement->expression);
	if(isGlobalVariable) {
		Instruction_pops(assignmentStatement->id->id, FRAME_GLOBAL);
	} else {
		Instruction_pops(assignmentStatement->id->id, codegen->frame);
	}
}

void __Codegen_directAssignment(const Codegen *codegen, const AssignmentStatementASTNode *assignmentStatement, bool isGlobalVariable) {
	LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)assignmentStatement->expression;
	enum Frame currentFrame = isGlobalVariable ? FRAME_GLOBAL : codegen->frame;

	switch(literal->type.type) {
		case TYPE_INT:
			Instruction_move_int_id(currentFrame, assignmentStatement->id->id, literal->value.integer);
			break;
		case TYPE_DOUBLE:
			Instruction_move_float_id(currentFrame, assignmentStatement->id->id, literal->value.floating);
			break;
		case TYPE_BOOL:
			Instruction_move_bool_id(currentFrame, assignmentStatement->id->id, literal->value.boolean);
			break;
		case TYPE_STRING:
			Instruction_move_string_id(currentFrame, assignmentStatement->id->id, literal->value.string);
			break;
		case TYPE_NIL:
			Instruction_move_nil_id(currentFrame, assignmentStatement->id->id);
			break;
		case TYPE_VOID:
		case TYPE_UNKNOWN:
		case TYPE_INVALID:
			fassertf("[Codegen] Unknown or invalid literal type.");
	}
}

void __Codegen_evaluateAssignmentStatement(Codegen *codegen, AssignmentStatementASTNode *assignmentStatement) {
	bool isGlobalVariable = Analyser_isDeclarationGlobal(codegen->analyser, assignmentStatement->id->id);

	// Optimization shortcut
	if(assignmentStatement->expression->_type == NODE_LITERAL_EXPRESSION) {
		__Codegen_directAssignment(codegen, assignmentStatement, isGlobalVariable);
	} else {
		__Codegen_stackAssignment(codegen, assignmentStatement, isGlobalVariable);
	}

	NEWLINE
}

void __Codegen_evaluateExpressionStatement(Codegen *codegen, ExpressionStatementASTNode *expressionStatement) {
	__Codegen_evaluateExpression(codegen, expressionStatement->expression);
	Instruction_clears();
}

void __Codegen_evaluateExpression(Codegen *codegen, ExpressionASTNode *expression) {
	assertf(codegen != NULL);
	assertf(expression != NULL);

	switch(expression->_type) {
		case NODE_LITERAL_EXPRESSION: {
			LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)expression;
			__Codegen_evaluateLiteral(literal);
		} break;
		case NODE_IDENTIFIER: {
			IdentifierASTNode *identifier = (IdentifierASTNode*)expression;
			if(Analyser_isDeclarationGlobal(codegen->analyser, identifier->id)) {
				Instruction_pushs_var(identifier->id, FRAME_GLOBAL);
			} else {
				Instruction_pushs_var(identifier->id, codegen->frame);
			}
		} break;
		case NODE_FUNCTION_CALL: {
			FunctionCallASTNode *functionCall = (FunctionCallASTNode*)expression;
			enum BuiltInFunction builtin = Analyser_getBuiltInFunctionById(codegen->analyser, functionCall->id->id);
			if(is_func_builtin(builtin)) {
				__Codegen_resolveBuiltInFunction(codegen, functionCall, builtin);
			} else {
				__Codegen_evaluateFunctionCall(codegen, functionCall);
			}
		} break;
		case NODE_UNARY_EXPRESSION: {
			UnaryExpressionASTNode *unaryExpression = (UnaryExpressionASTNode*)expression;
			__Codegen_evaluateExpression(codegen, unaryExpression->argument);
            if(unaryExpression->operator == OPERATOR_NOT) {
                Instruction_nots();
            }
		} break;
		case NODE_BINARY_EXPRESSION: {
			BinaryExpressionASTNode *binaryExpression = (BinaryExpressionASTNode*)expression;
			__Codegen_evaluateBinaryExpression(codegen, binaryExpression);
		} break;
		case NODE_INTERPOLATION_EXPRESSION: {
			InterpolationExpressionASTNode *interpolation = (InterpolationExpressionASTNode*)expression;
			assertf(interpolation);
			assertf(interpolation->concatenated);
			__Codegen_evaluateBinaryExpression(codegen, interpolation->concatenated);
		} break;
		default:
			fassertf("Unexpected ASTNode type. Analyser probably failed.");
	}
}

void __Codegen_evaluateBlock(Codegen *codegen, BlockASTNode *block) {
	Array *statements = block->statements;

	for(size_t i = 0; i < statements->size; ++i) {
		StatementASTNode *statement = Array_get(statements, i);
		__Codegen_evaluateStatement(codegen, statement);
	}
}

void __Codegen_resolveBuiltInFunction(Codegen *codegen, FunctionCallASTNode *functionCall, enum BuiltInFunction function) {
	switch(function) {
		case FUNCTION_READ_STRING: {
			Instruction_readString("READSTRING_TMP", FRAME_GLOBAL);
			Instruction_pushs_var_named("READSTRING_TMP", FRAME_GLOBAL);
		} break;
		case FUNCTION_READ_INT: {
			Instruction_readInt("READINT_TMP", FRAME_GLOBAL);
			Instruction_pushs_var_named("READINT_TMP", FRAME_GLOBAL);
		} break;
		case FUNCTION_READ_DOUBLE: {
			Instruction_readFloat("READDOUBLE_TMP", FRAME_GLOBAL);
			Instruction_pushs_var_named("READDOUBLE_TMP", FRAME_GLOBAL);
		} break;
		case FUNCTION_WRITE: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			for(size_t i = 0; i < argumentList->arguments->size; ++i) {
				ArgumentASTNode *argument = Array_get(argumentList->arguments, i);
				__Codegen_evaluateExpression(codegen, argument->expression);
				Instruction_pops_where("WRITE_TMP", FRAME_GLOBAL);
				Instruction_write("WRITE_TMP", FRAME_GLOBAL);
			}
		} break;
		case FUNCTION_INT_TO_DOUBLE: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			ArgumentASTNode *argument = Array_get(argumentList->arguments, 0);
			__Codegen_evaluateExpression(codegen, argument->expression);
			Instruction_int2floats();
		} break;
		case FUNCTION_DOUBLE_TO_INT: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			ArgumentASTNode *argument = Array_get(argumentList->arguments, 0);
			__Codegen_evaluateExpression(codegen, argument->expression);
			Instruction_float2ints();
		} break;
		case FUNCTION_LENGTH: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			ArgumentASTNode *argument = Array_get(argumentList->arguments, 0);
			__Codegen_evaluateExpression(codegen, argument->expression);

			// Overhead to call strlen
			Instruction_createframe();
			Instruction_defvar_where("ARG1_LEN", FRAME_TEMPORARY);
			Instruction_pops_where("ARG1_LEN", FRAME_TEMPORARY);

			// Call function
			Instruction_call("length");

			// Handle return value
			Instruction_pushs_var_named("RETVAL_LEN", FRAME_TEMPORARY);
		} break;
		case FUNCTION_SUBSTRING: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			ArgumentASTNode *string = Array_get(argumentList->arguments, 0);
			ArgumentASTNode *index_i = Array_get(argumentList->arguments, 1);
			ArgumentASTNode *index_j = Array_get(argumentList->arguments, 2);

			__Codegen_evaluateExpression(codegen, string->expression);
			__Codegen_evaluateExpression(codegen, index_i->expression);
			__Codegen_evaluateExpression(codegen, index_j->expression);

			// Overhead to call substr
			Instruction_createframe();
			Instruction_defvar_where("ARG1_SUBSTR", FRAME_TEMPORARY);
			Instruction_defvar_where("ARG2_SUBSTR", FRAME_TEMPORARY);
			Instruction_defvar_where("ARG3_SUBSTR", FRAME_TEMPORARY);

			Instruction_pops_where("ARG3_SUBSTR", FRAME_TEMPORARY);
			Instruction_pops_where("ARG2_SUBSTR", FRAME_TEMPORARY);
			Instruction_pops_where("ARG1_SUBSTR", FRAME_TEMPORARY);

			// Call function
			Instruction_call("substr");

			// Handle return value
			Instruction_pushs_var_named("RETVAL_SUBSTR", FRAME_TEMPORARY);
		} break;
		case FUNCTION_ORD: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			ArgumentASTNode *argument = Array_get(argumentList->arguments, 0);
			__Codegen_evaluateExpression(codegen, argument->expression);

			// Overhead to call ord
			Instruction_createframe();
			Instruction_defvar_where("ARG1_ORD", FRAME_TEMPORARY);
			Instruction_pops_where("ARG1_ORD", FRAME_TEMPORARY);

			// Call function
			Instruction_call("ord");

			// Handle return value
			Instruction_pushs_var_named("RETVAL_ORD", FRAME_TEMPORARY);
		} break;
		case FUNCTION_CHR: {
			ArgumentListASTNode *argumentList = functionCall->argumentList;
			ArgumentASTNode *argument = Array_get(argumentList->arguments, 0);
			__Codegen_evaluateExpression(codegen, argument->expression);

			// Overhead to call chr
			Instruction_createframe();
			Instruction_defvar_where("ARG1_CHR", FRAME_TEMPORARY);
			Instruction_pops_where("ARG1_CHR", FRAME_TEMPORARY);

			// Call function
			Instruction_call("chr");

			// Handle return value
			Instruction_pushs_var_named("RETVAL_CHR", FRAME_TEMPORARY);
		} break;
		case FUNCTION_INTERNAL_STRINGIFY_NUMBER:
		case FUNCTION_INTERNAL_STRINGIFY_DOUBLE:
		case FUNCTION_INTERNAL_STRINGIFY_INT:
		case FUNCTION_INTERNAL_STRINGIFY_BOOL:
		case FUNCTION_INTERNAL_STRINGIFY_STRING:
		case FUNCTION_INTERNAL_MODULO: {
			// Internal functions are handled as user defined functions
			__Codegen_evaluateFunctionCall(codegen, functionCall);
		} break;
		case FUNCTIONS_COUNT:
		case FUNCTION_NONE:
			fassertf("Expected builtin function, got user defined.");
	}
}

void __Codegen_evaluateFunctionCall(Codegen *codegen, FunctionCallASTNode *functionCall) {
	Array *arguments = functionCall->argumentList->arguments;

	FunctionDeclaration *functionDeclaration = Analyser_getFunctionById(codegen->analyser, functionCall->id->id);
	assertf(functionDeclaration != NULL, "Could not find function declaration with id %ld", functionCall->id->id);

	Array *parameters = functionDeclaration->node->parameterList->parameters;

	for(size_t i = 0; i < arguments->size; ++i) {
		ArgumentASTNode *argument = Array_get(arguments, i);

		__Codegen_evaluateExpression(codegen, argument->expression);
	}

	Instruction_createframe();

	for(int i = arguments->size - 1; i > -1; --i) {
		ParameterASTNode *parameter = Array_get(parameters, i);
		size_t parameterId = parameter->internalId->id;

		Instruction_defvar(parameterId, FRAME_TEMPORARY);
		Instruction_pops(parameterId, FRAME_TEMPORARY);
	}


	if(functionDeclaration->returnType.type != TYPE_VOID) {
		Instruction_defretvar(functionCall->id->id, FRAME_TEMPORARY);
	}

	Instruction_call_func(functionCall->id->id);

	if(functionDeclaration->returnType.type != TYPE_VOID) {
		Instruction_pushs_func_result(functionCall->id->id);
	}
}

void __Codegen_evaluateForStatement(Codegen *codegen, ForStatementASTNode *forStatement) {
	Instruction_label("for_cycle");

	__Codegen_evaluateExpression(codegen, forStatement->range->start);
	Instruction_defvar(forStatement->iterator->id, codegen->frame);
	Instruction_pops(forStatement->iterator->id, codegen->frame);

	Instruction_defvar_where("for_end", codegen->frame);
	__Codegen_evaluateExpression(codegen, forStatement->range->end);
	Instruction_pops_where("for_end", codegen->frame);

	Instruction_jump("for_cycle_test");

	Instruction_label("for_cycle_iterate");
	// Increment iterator
	Instruction_add_int_id(codegen->frame, forStatement->iterator->id, codegen->frame, forStatement->iterator->id, 1);

	Instruction_label("for_cycle_test");
	if(forStatement->range->operator == OPERATOR_LESS) {
		Instruction_pushs_var(forStatement->iterator->id, codegen->frame);
		Instruction_pushs_var_named("for_end", codegen->frame);
		Instruction_lts();
	} else {
		Instruction_pushs_var(forStatement->iterator->id, codegen->frame);
		Instruction_pushs_var_named("for_end", codegen->frame);
		Instruction_gts();
		Instruction_nots();
	}

	Instruction_pushs_bool(true);
	Instruction_jump_ifeqs("for_cycle_end");

	Instruction_label("for_cycle_start");
	__Codegen_evaluateBlock(codegen, forStatement->body);
	Instruction_jump("for_cycle_iterate");

	Instruction_label("for_cycle_end");
}

/** End of file src/compiler/codegen/Codegen.c **/
