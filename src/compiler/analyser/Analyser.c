#include "compiler/analyser/Analyser.h"

#include "allocator/MemoryAllocator.h"
#include "internal/Array.h"
#include "internal/HashMap.h"
#include "compiler/analyser/AnalyserResult.h"
#include "compiler/lexer/Token.h"


String* __Analyser_stringifyType(ValueType type);
void __Analyser_createBlockScopeChaining(Analyser *analyser, BlockASTNode *block, BlockScope *parent);
void __Analyser_createBlockScopeChaining_processNode(Analyser *analyser, ASTNode *node, BlockScope *parent);
AnalyserResult __Analyser_analyseBlock(Analyser *analyser, BlockASTNode *block);
AnalyserResult __Analyser_collectFunctionDeclarations(Analyser *analyser);


ValueType Analyser_getTypeFromToken(enum TokenKind tokenKind) {
	switch(tokenKind) {
		case TOKEN_INTEGER: return (ValueType){.type = TYPE_INT, .isNullable = false};
		case TOKEN_FLOATING: return (ValueType){.type = TYPE_DOUBLE, .isNullable = false};
		case TOKEN_BOOLEAN: return (ValueType){.type = TYPE_BOOL, .isNullable = false};
		case TOKEN_STRING: return (ValueType){.type = TYPE_STRING, .isNullable = false};
		case TOKEN_NIL: return (ValueType){.type = TYPE_NIL, .isNullable = true};
		default: return (ValueType){.type = TYPE_INVALID, .isNullable = false};
	}
}

VariableDeclaration* new_VariableDeclaration(
	Analyser *analyser,
	VariableDeclaratorASTNode *node,
	bool isConstant,
	ValueType type,
	String *name,
	bool isUserDefined,
	bool isInitialized
) {
	VariableDeclaration *declaration = (VariableDeclaration*)mem_alloc(sizeof(VariableDeclaration));

	declaration->_type = DECLARATION_VARIABLE;
	declaration->id = analyser ? analyser->idCounter++ : 0;
	declaration->isConstant = isConstant;
	declaration->isUsed = false;

	if(node) {
		declaration->name = node->pattern->id->name;
		// declaration->type = node->pattern->type ? node->pattern->type->id->name : NULL;
		// declaration->isNullable = node->pattern->type ? node->pattern->type->isNullable : false;
		declaration->type.type = node->pattern->type ? Analyser_resolveBuiltInType(node->pattern->type->id->name) : TYPE_UNKNOWN;
		declaration->type.isNullable = node->pattern->type ? node->pattern->type->isNullable : false;
		declaration->isUserDefined = true;
		declaration->isInitialized = node->initializer != NULL;
	} else {
		declaration->name = name;
		declaration->type = type;
		declaration->isUserDefined = isUserDefined;
		declaration->isInitialized = isInitialized;
	}

	// Add declaration to the ids pool
	HashMap_set(analyser->idsPool, String_fromLong(declaration->id)->value, declaration);

	return declaration;
}

FunctionDeclaration* new_FunctionDeclaration(Analyser *analyser, FunctionDeclarationASTNode *node) {
	FunctionDeclaration *declaration = (FunctionDeclaration*)mem_alloc(sizeof(FunctionDeclaration));

	declaration->_type = DECLARATION_FUNCTION;
	declaration->id = analyser ? analyser->idCounter++ : 0;
	declaration->node = node;
	declaration->variables = HashMap_alloc();
	declaration->isUsed = false;

	// Add declaration to the ids pool
	HashMap_set(analyser->idsPool, String_fromLong(declaration->id)->value, declaration);

	return declaration;
}


void VariableDeclaration_free(VariableDeclaration *declaration) {
	mem_free(declaration);
}

void FunctionDeclaration_free(FunctionDeclaration *declaration) {
	HashMap_free(declaration->variables);
	mem_free(declaration);
}


BlockScope* BlockScope_alloc(BlockScope *parent) {
	BlockScope *scope = (BlockScope*)mem_alloc(sizeof(BlockScope));
	scope->parent = parent;
	scope->variables = HashMap_alloc();
	return scope;
}

void BlockScope_free(BlockScope *scope) {
	HashMap_free(scope->variables);
	mem_free(scope);
}

void Analyser_constructor(Analyser *analyser) {
	analyser->globalScope = BlockScope_alloc(NULL);
	analyser->functions = HashMap_alloc();
	analyser->variables = HashMap_alloc();
	analyser->idsPool = HashMap_alloc();
	analyser->idCounter = 1;
	analyser->ast = NULL;

	// TODO: probably delete this
	analyser->types = HashMap_alloc();
	HashMap_set(analyser->types, "Int", String_alloc("Int"));
	HashMap_set(analyser->types, "Double", String_alloc("Double"));
	HashMap_set(analyser->types, "Bool", String_alloc("Bool"));
	HashMap_set(analyser->types, "String", String_alloc("String"));
	HashMap_set(analyser->types, "Void", String_alloc("Void"));
}

void Analyser_destructor(Analyser *analyser) {
	if(analyser->globalScope) BlockScope_free(analyser->globalScope);
	if(analyser->functions) HashMap_free(analyser->functions);
	if(analyser->variables) HashMap_free(analyser->variables);
	if(analyser->idsPool) HashMap_free(analyser->idsPool);
	if(analyser->types) HashMap_free(analyser->types);
	analyser->idCounter = 0;
}

Declaration* Analyser_getDeclarationById(Analyser *analyser, size_t id) {
	return HashMap_get(analyser->idsPool, String_fromLong(id)->value);
}

FunctionDeclaration* Analyser_getFunctionById(Analyser *analyser, size_t id) {
	Declaration *declaration = Analyser_getDeclarationById(analyser, id);
	if(!declaration) return NULL;
	if(declaration->_type != DECLARATION_FUNCTION) return NULL;

	return (FunctionDeclaration*)declaration;
}

VariableDeclaration* Analyser_getVariableById(Analyser *analyser, size_t id) {
	Declaration *declaration = Analyser_getDeclarationById(analyser, id);
	if(!declaration) return NULL;
	if(declaration->_type != DECLARATION_VARIABLE) return NULL;

	return (VariableDeclaration*)declaration;
}

VariableDeclaration* Analyser_getVariableByName(Analyser *analyser, String *name, BlockScope *scope) {
	VariableDeclaration *declaration = NULL;

	(void)analyser;

	while(scope) {
		declaration = HashMap_get(scope->variables, name->value);
		if(declaration) return declaration;

		scope = scope->parent;
	}

	return NULL;
}

enum BuiltInTypes Analyser_resolveBuiltInType(String *name) {
	if(String_equals(name, "Int")) return TYPE_INT;
	if(String_equals(name, "Double")) return TYPE_DOUBLE;
	if(String_equals(name, "Bool")) return TYPE_BOOL;
	if(String_equals(name, "String")) return TYPE_STRING;
	if(String_equals(name, "Void")) return TYPE_VOID;
	return TYPE_INVALID;
}

ValueType Analyser_TypeReferenceToValueType(TypeReferenceASTNode *node) {
	ValueType type = (ValueType){
		.type = Analyser_resolveBuiltInType(node->id->name),
		.isNullable = node->isNullable
	};

	return type;
}

AnalyserResult Analyser_analyse(Analyser *analyser, ProgramASTNode *ast) {
	// Reconstruct the analyser
	Analyser_destructor(analyser);
	Analyser_constructor(analyser);

	analyser->ast = ast;

	__Analyser_createBlockScopeChaining(analyser, ast->block, analyser->globalScope);
	__Analyser_collectFunctionDeclarations(analyser);

	return __Analyser_analyseBlock(analyser, ast->block);
}



String* __Analyser_stringifyType(ValueType type) {
	String *str = String_alloc("");

	switch(type.type) {
		case TYPE_INT: String_set(str, "Int"); break;
		case TYPE_DOUBLE: String_set(str, "Double"); break;
		case TYPE_BOOL: String_set(str, "Bool"); break;
		case TYPE_STRING: String_set(str, "String"); break;
		case TYPE_VOID: String_set(str, "Void"); break;
		case TYPE_NIL: String_set(str, "Nil"); break;
		case TYPE_UNKNOWN: String_set(str, "Unknown"); break;
		case TYPE_INVALID: String_set(str, "Invalid"); break;
	}

	if(type.isNullable) String_appendChar(str, '?');

	return str;
}

char* __Analyser_stringifyOperator(OperatorType operator) {
	switch(operator) {
		case OPERATOR_PLUS: return "+";
		case OPERATOR_MINUS: return "-";
		case OPERATOR_MUL: return "*";
		case OPERATOR_DIV: return "/";
		case OPERATOR_UNWRAP: return "!";
		case OPERATOR_NULL_COALESCING: return "??";
		case OPERATOR_EQUAL: return "==";
		case OPERATOR_NOT_EQUAL: return "!=";
		case OPERATOR_LESS: return "<";
		case OPERATOR_LESS_EQUAL: return "<=";
		case OPERATOR_GREATER: return ">";
		case OPERATOR_GREATER_EQUAL: return ">=";
		default: return "Unknown";
	}
}

void __Analyser_createBlockScopeChaining(Analyser *analyser, BlockASTNode *block, BlockScope *parent) {
	block->scope = BlockScope_alloc(parent);

	for(size_t i = 0; i < block->statements->size; i++) {
		StatementASTNode *statement = Array_get(block->statements, i);

		__Analyser_createBlockScopeChaining_processNode(analyser, statement, block->scope);
	}
}

void __Analyser_createBlockScopeChaining_processNode(Analyser *analyser, ASTNode *node, BlockScope *parent) {
	if(node->_type != NODE_BLOCK) return;

	switch(node->_type) {
		case NODE_IF_STATEMENT: {
			IfStatementASTNode *ifStatement = (IfStatementASTNode*)node;
			__Analyser_createBlockScopeChaining(analyser, ifStatement->body, parent);

			if(ifStatement->alternate) {
				if(ifStatement->alternate->_type == NODE_BLOCK) {
					BlockASTNode *alternate = (BlockASTNode*)ifStatement->alternate;
					__Analyser_createBlockScopeChaining(analyser, alternate, parent);
				} else {
					__Analyser_createBlockScopeChaining_processNode(analyser, ifStatement->alternate, parent);
				}
			}
		} break;

		case NODE_WHILE_STATEMENT: {
			WhileStatementASTNode *whileStatement = (WhileStatementASTNode*)node;
			__Analyser_createBlockScopeChaining(analyser, whileStatement->body, parent);
		} break;

		case NODE_FUNCTION_DECLARATION: {
			FunctionDeclarationASTNode *function = (FunctionDeclarationASTNode*)node;
			__Analyser_createBlockScopeChaining(analyser, function->body, parent);
		} break;

		default: {
			// No other nodes can contain a BlockASTNode
		} break;
	}
}

AnalyserResult __Analyser_analyseBlock(Analyser *analyser, BlockASTNode *block) {
	for(size_t i = 0; i < block->statements->size; i++) {
		StatementASTNode *statement = Array_get(block->statements, i);

		switch(statement->_type) {
			case NODE_VARIABLE_DECLARATION: {
				VariableDeclarationASTNode *declarationNode = (VariableDeclarationASTNode*)statement;
				Array *declarators = declarationNode->declaratorList->declarators;

				for(size_t j = 0; j < declarators->size; j++) {
					VariableDeclaratorASTNode *declaratorNode = Array_get(declarators, j);

					// In case the initializer is absent, the type annotation is required
					// TODO: This might be also a syntax error?
					if(!declaratorNode->initializer && !declaratorNode->pattern->type) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat("type annotation missing in pattern"),
							NULL
						);
					}

					// Cannot resolve type (provided type is not supported)
					// TODO: This might be a syntax error as well? Others probably implemented it fixed, using grammar rules
					if(declaratorNode->pattern->type && !HashMap_has(analyser->types, declaratorNode->pattern->type->id->name->value)) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat("cannot find type '%s' in scope", declaratorNode->pattern->type->id->name->value),
							NULL
						);
					}

					VariableDeclaration *declaration = new_VariableDeclaration(
						analyser,
						declaratorNode,
						declarationNode->isConstant,
						(ValueType){0}, NULL, false, false
					);
					declaratorNode->pattern->id->id = declaration->id;

					// Validate/infer the type based on the initializer
					if(declaratorNode->initializer) {
						// TODO: Get type of the initializer
						ValueType type;
						AnalyserResult result = Analyser_resolveExpressionType(analyser, declaratorNode->initializer, block->scope, &type);
						if(!result.success) return result;

						// TODO: Check if the type of the initializer matches the type annotation
						if(declaratorNode->pattern->type) {
							declaratorNode->pattern->type->type = Analyser_TypeReferenceToValueType(declaratorNode->pattern->type);

							// TODO: Convert Int to Double if needed
							if(!is_value_assignable(declaratorNode->pattern->type->type, type)) {
								return AnalyserError(
									RESULT_ERROR_SEMANTIC_INVALID_TYPE,
									String_fromFormat(
										"cannot convert value of type '%s' to specified type '%s'",
										__Analyser_stringifyType(type)->value,
										__Analyser_stringifyType(declaratorNode->pattern->type->type)->value
									),
									NULL
								);
							}
						} else {
							declaration->type = type;
						}
					}

					VariableDeclaration *existingDeclaration = HashMap_get(block->scope->variables, declaration->name->value);

					// There is already a variable with the same name in the current scope
					if(existingDeclaration && existingDeclaration->isUserDefined) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat("invalid redeclaration of '%s'", declaration->name->value),
							NULL
						);
					}

					HashMap_set(block->scope->variables, declaration->name->value, declaration);
				}
			} break;

			case NODE_ASSIGNMENT_STATEMENT: {
				AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;
				VariableDeclaration *variable = Analyser_getVariableByName(analyser, assignment->id->name, block->scope);

				// Variable is not declared in reachable scopes
				if(!variable) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER,
						String_fromFormat("cannot find '%s' in scope", assignment->id->name),
						NULL
					);
				}

				// Constant variable is already initialized
				if(variable->isConstant && variable->isInitialized) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER,
						String_fromFormat("cannot assign to constant '%s'", assignment->id->name),
						NULL
					);
				}

				// TODO: Check for type compatibility

				assignment->id->id = variable->id;
			} break;

			default: {
				warnf("TODO: Analyse statement %d", statement->_type);
			} break;
		}

	}

	// TODO: Uncomment this to print out all unhandled nodes
	return AnalyserSuccess();
}

AnalyserResult Analyser_resolveExpressionType(Analyser *analyser, ExpressionASTNode *node, BlockScope *scope, ValueType *outType) {
	switch(node->_type) {
		case NODE_LITERAL_EXPRESSION: {
			// Literals are handled by parser
			*outType = ((LiteralExpressionASTNode*)node)->type;
		} break;

		case NODE_IDENTIFIER: {
			IdentifierASTNode *identifier = (IdentifierASTNode*)node;

			// This identifier node was already resolved
			if(identifier->id != 0) {
				VariableDeclaration *declaration = Analyser_getVariableById(analyser, identifier->id);
				assertf(declaration);

				*outType = declaration->type;
				return AnalyserSuccess();
			}

			VariableDeclaration *declaration = Analyser_getVariableByName(analyser, identifier->name, scope);

			if(!declaration) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE,
					String_fromFormat("cannot find '%s' in scope", identifier->name->value),
					NULL
				);
			}

			if(declaration->type.type == TYPE_UNKNOWN) {
				// TODO: Maybe comment out the assert before submitting
				fassertf("Cannot infer type of the variable (this should never happen!)");

				return AnalyserError(
					RESULT_ERROR_SEMANTIC_FAILED_INFER,
					String_fromFormat("cannot infer type of '%s'", identifier->name->value),
					NULL
				);
			}

			*outType = declaration->type;
		} break;

		case NODE_FUNCTION_CALL: {
			FunctionCallASTNode *call = (FunctionCallASTNode*)node;

			// This function call node was already resolved
			if(call->id->id != 0) {
				FunctionDeclaration *declaration = Analyser_getFunctionById(analyser, call->id->id);
				assertf(declaration);

				*outType = declaration->returnType;
				return AnalyserSuccess();
			}

			Array /*<FunctionDeclaration>*/ *declarations = HashMap_get(analyser->functions, call->id->name->value);

			if(!declarations) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_FUNCTION_DEFINITION,
					String_fromFormat("cannot find '%s' in scope", call->id->name->value),
					NULL
				);
			}

			// Resolve overload
			// TODO: Resolve types of the arguments
			// TODO: Match the resolved types with the available overloads

			Array /*<ArgumentASTNode>*/ *arguments = call->argumentList->arguments;

			for(size_t i = 0; i < declarations->size; i++) {
				FunctionDeclaration *declaration = Array_get(declarations, i);

				if(declaration->node->parameterList->parameters->size != arguments->size) continue;

				bool match = true;

				Array /*<ArgumentASTNode>*/ *arguments = call->argumentList->arguments;

				for(size_t j = 0; j < arguments->size; j++) {
					ArgumentASTNode *argument = Array_get(arguments, j);
					ParameterASTNode *parameter = Array_get(declaration->node->parameterList->parameters, j);

					ValueType argumentType;
					AnalyserResult result = Analyser_resolveExpressionType(analyser, argument->expression, scope, &argumentType);
					if(!result.success) return result;

					if(argumentType.type != parameter->type->type.type) {
						match = false;
						break;
					}
				}

				if(!match) continue;

				call->id->id = declaration->id;
				*outType = declaration->returnType;

				return AnalyserSuccess();
			}

			return AnalyserError(
				RESULT_ERROR_SEMANTIC_FUNCTION_DEFINITION,
				String_fromFormat("no exact matches in call to global function '%s'", call->id->name->value),
				NULL
			);
		} break;

		case NODE_UNARY_EXPRESSION: {
			UnaryExpressionASTNode *unary = (UnaryExpressionASTNode*)node;

			ValueType type;
			AnalyserResult result = Analyser_resolveExpressionType(analyser, unary->argument, scope, &type);
			if(!result.success) return result;

			switch(unary->operator) {
				case OPERATOR_UNWRAP: {
					if(!type.isNullable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat("cannot force unwrap value of non-optional type '%s'", __Analyser_stringifyType(type)->value),
							NULL
						);
					}

					unary->type = (ValueType){.type = type.type, .isNullable = false};
				} break;

				default: {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER,
						String_fromFormat("'%s' is not a %s unary operator", unary->operator, /*unary.isPrefix ? "prefix" :*/ "postfix"),
						NULL
					);
				} break;
			}

			*outType = unary->type;
		} break;

		case NODE_BINARY_EXPRESSION: {
			BinaryExpressionASTNode *binary = (BinaryExpressionASTNode*)node;

			ValueType leftType;
			AnalyserResult result = Analyser_resolveExpressionType(analyser, binary->left, scope, &leftType);
			if(!result.success) return result;

			ValueType rightType;
			result = Analyser_resolveExpressionType(analyser, binary->right, scope, &rightType);
			if(!result.success) return result;

			switch(binary->operator) {
				case OPERATOR_PLUS:
				case OPERATOR_MINUS:
				case OPERATOR_MUL:
				case OPERATOR_DIV: {
					// Check for null types
					if(leftType.isNullable || rightType.isNullable) {
						String *typeName = __Analyser_stringifyType((ValueType){
							.type = leftType.isNullable ? leftType.type : rightType.type,
							.isNullable = false
						});

						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat("value of optional type '%s?' must be unwrapped to a value of type '%s'", typeName->value, typeName->value),
							NULL
						);
					}

					// Resolve the overload and cast types of the literals
					if(binary->operator == OPERATOR_PLUS && leftType.type == TYPE_STRING && rightType.type == TYPE_STRING) {
						binary->type.type = TYPE_STRING;
					} else if(leftType.type == TYPE_INT && rightType.type == TYPE_INT) {
						binary->type.type = TYPE_INT;
					} else if(leftType.type == TYPE_DOUBLE && rightType.type == TYPE_DOUBLE) {
						binary->type.type = TYPE_DOUBLE;
					} else if(
						binary->operator != OPERATOR_DIV &&
						binary->left->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_INT && rightType.type == TYPE_DOUBLE
					) {
						LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->left;

						literal->value.floating = (double)literal->value.integer;
						literal->type.type = TYPE_DOUBLE;

						binary->type.type = TYPE_DOUBLE;
					} else if(
						binary->operator != OPERATOR_DIV &&
						binary->right->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_DOUBLE && rightType.type == TYPE_INT
					) {
						LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->right;

						literal->value.floating = (double)literal->value.integer;
						literal->type.type = TYPE_DOUBLE;

						binary->type.type = TYPE_DOUBLE;
					} else {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"binary operator '%s' cannot be applied to operands of type '%s' and '%s'",
								__Analyser_stringifyOperator(binary->operator),
								__Analyser_stringifyType(leftType)->value,
								__Analyser_stringifyType(rightType)->value
							),
							NULL
						);
					}

					binary->type.isNullable = false;
					outType->type = binary->type.type;
				} break;

				case OPERATOR_EQUAL:
				case OPERATOR_NOT_EQUAL: {
					/*
					   Pro operátor == platí: Pokud je první operand jiného typu než druhý operand, dochází
					   k chybě 7. Číselné literály jsou při sémantických kontrolách implicitně přetypovány na potřebný číselný typ (tj. z Int na Double), avšak u proměnných k implicitním konverzím
					   nedochází. Pokud jsou operandy stejného typu, tak se porovnají hodnoty daných operandů.
					   Operátor != je negací operátoru ==.
					 */

					// Cast types of the literals
					if(leftType.type == rightType.type) {
						binary->type.type = TYPE_BOOL;
					} else if(
						binary->left->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_INT && rightType.type == TYPE_DOUBLE
					) {
						LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->left;

						literal->value.floating = (double)literal->value.integer;
						literal->type.type = TYPE_DOUBLE;

						binary->type.type = TYPE_BOOL;
					} else if(
						binary->right->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_DOUBLE && rightType.type == TYPE_INT
					) {
						LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->right;

						literal->value.floating = (double)literal->value.integer;
						literal->type.type = TYPE_DOUBLE;

						binary->type.type = TYPE_BOOL;
					} else {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"binary operator '%s' cannot be applied to operands of type '%s' and '%s'",
								__Analyser_stringifyOperator(binary->operator),
								__Analyser_stringifyType(leftType)->value,
								__Analyser_stringifyType(rightType)->value
							),
							NULL
						);
					}

					binary->type = (ValueType){
						.type = TYPE_BOOL,
						.isNullable = leftType.isNullable || rightType.isNullable
					};
					*outType = binary->type;
				} break;

				case OPERATOR_LESS:
				case OPERATOR_LESS_EQUAL:
				case OPERATOR_GREATER:
				case OPERATOR_GREATER_EQUAL: {
					/*
					   Pro relační operátory <, >, <=, >= platí: Sémantika operátorů odpovídá jazyku Swift.
					   Nelze při porovnání mít jeden z operandů jiného typu nebo druhý (např. jeden celé a druhý
					   desetinné číslo), ani potenciálně obsahující nil (tj. případný výraz je nejprve potřeba převést na výraz typu bez hodnoty nil). U řetězců se porovnání provádí lexikograficky. Všechny
					   typové nekompatibility ve výrazech jsou v IFJ23 hlášeny jako sémantická chyba.
					 */

					if(leftType.type != rightType.type) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"binary operator '%s' cannot be applied to operands of type '%s' and '%s'",
								__Analyser_stringifyOperator(binary->operator),
								__Analyser_stringifyType(leftType)->value,
								__Analyser_stringifyType(rightType)->value
							),
							NULL
						);
					}

					if(leftType.isNullable || rightType.isNullable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat("cannot use relational operator '%s' with optional type '%s'", __Analyser_stringifyOperator(binary->operator), __Analyser_stringifyType(leftType)->value),
							NULL
						);
					}

					binary->type = (ValueType){.type = TYPE_BOOL, .isNullable = false};
					*outType = binary->type;
				} break;

				case OPERATOR_NULL_COALESCING: {
					if(leftType.type != rightType.type) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"binary operator '%s' cannot be applied to operands of type '%s' and '%s'",
								__Analyser_stringifyOperator(binary->operator),
								__Analyser_stringifyType(leftType)->value,
								__Analyser_stringifyType(rightType)->value
							),
							NULL
						);
					}

					if(rightType.isNullable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_OTHER,
							String_fromFormat(
								"cannot use '%s' operator with optional type '%s' on right side",
								__Analyser_stringifyOperator(binary->operator),
								__Analyser_stringifyType(rightType)->value
							),
							NULL
						);
					}

					if(!leftType.isNullable) {
						// TODO: Maybe error here? The assignment lacks this edge case...
					}

					if(leftType.isNullable) {
						binary->type.isNullable = true;
					} else {
						binary->type.isNullable = false;
					}

					binary->type.type = rightType.type;
					*outType = binary->type;
				} break;

				default: {
					fassertf("Failed to resolve binary operator: Unexpected operator %d", binary->operator);
				} break;
			}
		} break;

		default: {
			fassertf("Failed to resolve type of node: Unexpected node type %d", node->_type);
		} break;
	}

	return AnalyserSuccess();
}

// AnalyserResult __Analyser_collectGlobalVariables(Analyser *analyser) {
// 	for(int i = 0; i < analyser->ast->block->statements->size; i++) {
// 		StatementASTNode *statement = Array_get(analyser->ast->block->statements, i);

// 		if(statement->_type != NODE_VARIABLE_DECLARATION) continue;

// 		VariableDeclarationASTNode *declarationNode = (VariableDeclarationASTNode*)statement;
// 		Array *declarators = declarationNode->declaratorList->declarators;

// 		for(int j = 0; j < declarators->size; j++) {
// 			VariableDeclaratorASTNode *declaratorNode = Array_get(declarators, j);

// 			VariableDeclaration *declaration = new_VariableDeclaration(
// 				analyser,
// 				declaratorNode,
// 				declarationNode->isConstant,
// 				NULL, NULL, false, false, false
// 			);

// 			if(!declaratorNode->initializer && !declaratorNode->pattern->type) {
// 				return AnalyserError(
// 					RESULT_ERROR_SEMANTIC_OTHER,
// 					String_fromFormat("type annotation missing in pattern"),
// 					NULL
// 				);
// 			}

// 			if(HashMap_has(analyser->globalScope->variables, declaration)) {
// 				return AnalyserError(
// 					RESULT_ERROR_SEMANTIC_OTHER,
// 					String_fromFormat("invalid redeclaration of '%s'", declaration->name),
// 					NULL
// 				);
// 			}

// 			HashMap_set(analyser->globalScope->variables, declaration->name, declaration);
// 		}
// 	}

// 	for(int i = 0; i < analyser->ast->block->statements->size; i++) {
// 		StatementASTNode *statement = Array_get(analyser->ast->block->statements, i);

// 		// 2. variable declarations, assignements
// 		// * declarations:
// 		// if !has type && !has initializer:
// 		//   Error
// 		// if has type && !has initializer:
// 		//   if is constant:
// 		//     Error
// 		//   else:
// 		//     OK
// 		// if has initializer:
// 		//   actual type = get type of initializer

// 		//   if has type:
// 		//     if type != actual type:
// 		//       Error
// 		//   else:
// 		//     if actual type == nil:
// 		//       Error
// 		//     else:
// 		//       set type to actual type
// 		//       OK

// 		// * assignements:
// 		// variable = resolve variable by name
// 		// actual type = get type of expression

// 		// if variable is constant:
// 		//   Error
// 		// else:
// 		//   if type != actual type:
// 		//     Error
// 		//   else:
// 		//     if is actual type nullable && !is type nullable:
// 		//       Error

// 		switch(statement->_type) {
// 			case NODE_VARIABLE_DECLARATION: {
// 				VariableDeclarationASTNode *declarationNode = (VariableDeclarationASTNode*)statement;
// 				Array *declarators = declarationNode->declaratorList->declarators;

// 				for(int j = 0; j < declarators->size; j++) {
// 					VariableDeclaratorASTNode *declaratorNode = Array_get(declarators, j);

// 					VariableDeclaration *declaration = new_VariableDeclaration(
// 						analyser,
// 						declaratorNode,
// 						declarationNode->isConstant,
// 						NULL, NULL, false, false, false
// 					);

// 					if(HashMap_has(analyser->globalScope->variables, declaration)) {
// 						return AnalyserError(
// 							RESULT_ERROR_SEMANTIC_OTHER,
// 							String_fromFormat("invalid redeclaration of '%s'", declaration->name),
// 							NULL
// 						);
// 					}

// 					HashMap_set(analyser->globalScope->variables, declaration->name, declaration);
// 				}
// 			} break;

// 			case NODE_EXPRESSION_STATEMENT: {
// 				ExpressionStatementASTNode *expressionStatement = (ExpressionStatementASTNode*)statement;
// 				ExpressionASTNode *expression = expressionStatement->expression;

// 				if(expression->_type != NODE_ASSIGNMENT_EXPRESSION) continue;

// 				AssignmentExpressionASTNode *assignment = (AssignmentExpressionASTNode*)expression;
// 				VariableDeclaration *variable = Analyser_resolveVariable(analyser, assignment->id->name, analyser->globalScope);

// 				if(!variable) {
// 					return AnalyserError(
// 						RESULT_ERROR_SEMANTIC_OTHER,
// 						String_fromFormat("use of undeclared identifier '%s'", assignment->id->name),
// 						NULL
// 					);
// 				}


// 			}
// 		}
// 	}
// }

AnalyserResult __Analyser_collectFunctionDeclarations(Analyser *analyser) {
	for(size_t i = 0; i < analyser->ast->block->statements->size; i++) {
		StatementASTNode *statement = Array_get(analyser->ast->block->statements, i);

		if(statement->_type != NODE_FUNCTION_DECLARATION) continue;

		// Create a new function declaration
		FunctionDeclarationASTNode *declarationNode = (FunctionDeclarationASTNode*)statement;
		FunctionDeclaration *declaration = new_FunctionDeclaration(analyser, declarationNode);

		if(declarationNode->returnType) {
			declaration->returnType.type = Analyser_resolveBuiltInType(declarationNode->returnType->id->name);
			declaration->returnType.isNullable = declarationNode->returnType->isNullable;

			if(!is_type_valid(declaration->returnType.type)) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_OTHER,
					String_fromFormat("cannot find type '%s' in scope", declarationNode->returnType->id->name->value),
					NULL
				);
			}

			declaration->returnType.isNullable = declarationNode->returnType->isNullable;
		} else {
			declaration->returnType = (ValueType){.type = TYPE_VOID, .isNullable = false};
		}

		// TODO: Analyse function declaration
		// Check for duplicate parameters
		{
			Array *parameterList = declarationNode->parameterList->parameters;
			HashMap *variables = declaration->node->body->scope->variables;

			// This will effectively check for duplicate parameters + register them as local variables
			for(size_t i = 0; i < parameterList->size; i++) {
				ParameterASTNode *parameter = Array_get(parameterList, i);
				String *name = parameter->internalId->name;

				if(!parameter->type) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER,
						String_fromFormat("type annotation missing in parameter '%s'", name),
						NULL
					);
				}

				String *typeName = parameter->internalId->name;
				enum BuiltInTypes resolvedType = Analyser_resolveBuiltInType(typeName);

				if(!is_type_valid(resolvedType)) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER,
						String_fromFormat("cannot find type '%s' in scope", typeName),
						NULL
					);
				}

				// Create a new variable declaration
				VariableDeclaration *variable = new_VariableDeclaration(
					analyser,
					NULL,
					true,
					(ValueType){.type = resolvedType, .isNullable = parameter->type->isNullable},
					name,
					false,
					true
				);

				// If the parameter is not in the hashmap yet
				if(!HashMap_has(variables, name->value)) {
					// Add the parameter to the hashmap
					HashMap_set(variables, name->value, variable);
					continue;
				}

				// There is already a parameter with the same name
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_OTHER,
					String_fromFormat("invalid redeclaration of '%s'", name),
					NULL
				);
			}
		}

		// Query the hashmap for the function declaration
		String *name = declarationNode->id->name;
		Array *overloads = HashMap_get(analyser->functions, name->value);

		// No overloads yet, create a new array and add the function declaration
		if(!overloads) {
			// Allocate a new array to store possible overloads
			overloads = Array_alloc(1);

			// Add the array to the hashmap
			HashMap_set(analyser->functions, name->value, overloads);

			// Add the function declaration to the array
			Array_push(overloads, declaration);

			continue;
		}

		// TODO: Check for overloads
		Array_push(overloads, declaration);
	}

	return AnalyserSuccess();
}
