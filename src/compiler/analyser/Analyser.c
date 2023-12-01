#include "compiler/analyser/Analyser.h"

#include "allocator/MemoryAllocator.h"
#include "internal/Array.h"
#include "internal/HashMap.h"
#include "internal/Utils.h"
#include "compiler/analyser/AnalyserResult.h"
#include "compiler/lexer/Token.h"
#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"


String* __Analyser_stringifyType(ValueType type);
void __Analyser_registerBuiltInFunctions(Analyser *analyser);
BlockScope* __Analyser_createBlockScopeChaining(Analyser *analyser, BlockASTNode *block, BlockScope *parent);
void __Analyser_createBlockScopeChaining_processNode(Analyser *analyser, ASTNode *node, BlockScope *parent);
AnalyserResult __Analyser_analyseBlock(Analyser *analyser, BlockASTNode *block);
AnalyserResult __Analyser_collectFunctionDeclarations(Analyser *analyser);
bool __Analyser_isReturnReachable_processNode(Analyser *analyser, StatementASTNode *node);
bool __Analyser_isReturnReachable(Analyser *analyser, BlockASTNode *block);


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

size_t Analyser_nextId(Analyser *analyser) {
	return analyser->idCounter++;
}

size_t Analyser_registerId(Analyser *analyser, size_t id) {
	assertf(id != 0, "Cannot register id 0");
	assertf(id < analyser->idCounter, "Cannot register id %zu, next available id is %zu", id, analyser->idCounter);

	analyser->idCounter = id + 1;

	return id;
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
	declaration->id = analyser ? Analyser_nextId(analyser) : 0;
	declaration->isConstant = isConstant;
	declaration->isUsed = false;

	if(node) {
		declaration->node = node;
		declaration->name = node->pattern->id->name;
		// declaration->type = node->pattern->type ? node->pattern->type->id->name : NULL;
		// declaration->isNullable = node->pattern->type ? node->pattern->type->isNullable : false;
		declaration->type.type = node->pattern->type ? Analyser_resolveBuiltInType(node->pattern->type->id->name) : TYPE_UNKNOWN;
		declaration->type.isNullable = node->pattern->type ? node->pattern->type->isNullable : false;
		declaration->isUserDefined = true;
		declaration->isInitialized = node->initializer != NULL;

		node->pattern->id->id = declaration->id;
	} else {
		declaration->node = NULL;
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
	declaration->id = analyser ? Analyser_nextId(analyser) : 0;
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
	scope->function = NULL;
	scope->loop = NULL;
	return scope;
}

void BlockScope_free(BlockScope *scope) {
	HashMap_free(scope->variables);
	mem_free(scope);
}

void Analyser_constructor(Analyser *analyser) {
	analyser->globalScope = BlockScope_alloc(NULL);
	analyser->overloads = HashMap_alloc();
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

bool Analyser_isDeclarationGlobal(Analyser *analyser, size_t id) {
	if(id == 0) return false;

	String *key = String_fromLong(id);
	bool isGlobal = HashMap_has(analyser->variables, key->value) || HashMap_has(analyser->functions, key->value);
	String_free(key);

	return isGlobal;
}

Declaration* Analyser_getDeclarationById(Analyser *analyser, size_t id) {
	if(id == 0) return NULL;

	String *key = String_fromLong(id);
	Declaration *declaration = HashMap_get(analyser->idsPool, key->value);
	String_free(key);
	return declaration;
}

FunctionDeclaration* Analyser_getFunctionById(Analyser *analyser, size_t id) {
	if(id == 0) return NULL;

	Declaration *declaration = Analyser_getDeclarationById(analyser, id);
	if(!declaration) return NULL;
	if(declaration->_type != DECLARATION_FUNCTION) return NULL;

	return (FunctionDeclaration*)declaration;
}

VariableDeclaration* Analyser_getVariableById(Analyser *analyser, size_t id) {
	if(id == 0) return NULL;

	Declaration *declaration = Analyser_getDeclarationById(analyser, id);
	if(!declaration) return NULL;
	if(declaration->_type != DECLARATION_VARIABLE) return NULL;

	return (VariableDeclaration*)declaration;
}

VariableDeclaration* Analyser_getVariableByName(Analyser *analyser, char *name, BlockScope *scope) {
	VariableDeclaration *declaration = NULL;

	(void)analyser;

	while(scope) {
		declaration = HashMap_get(scope->variables, name);
		if(declaration) return declaration;

		scope = scope->parent;
	}

	return NULL;
}

enum BuiltInFunction Analyser_getBuiltInFunctionById(Analyser *analyser, size_t id) {
	if(id == 0) return FUNCTION_NONE;

	// Get the function declaration
	FunctionDeclaration *declaration = Analyser_getFunctionById(analyser, id);
	if(!declaration) return FUNCTION_NONE;

	return declaration->node->builtin;
}

Array /*<FunctionDeclaration> | NULL*/* Analyser_getFunctionDeclarationsByName(Analyser *analyser, char *name) {
	Array *overloads = HashMap_get(analyser->overloads, name);
	if(!overloads) return NULL;

	return overloads;
}

FunctionDeclaration* Analyser_getNearestFunctionDeclaration(Analyser *analyser, BlockScope *scope) {
	(void)analyser;

	while(scope) {
		if(scope->function) return scope->function;

		scope = scope->parent;
	}

	return NULL;
}

StatementASTNode /*<ForStatementASTNode | WhileStatementASTNode>*/* Analyser_getNearestLoop(Analyser *analyser, BlockScope *scope) {
	(void)analyser;

	while(scope) {
		if(scope->loop) return scope->loop;

		scope = scope->parent;
	}

	return NULL;
}

bool __Analyser_isReturnReachable_processNode(Analyser *analyser, StatementASTNode *node) {
	switch(node->_type) {
		case NODE_RETURN_STATEMENT: return true;
		case NODE_IF_STATEMENT: {
			IfStatementASTNode *ifStatement = (IfStatementASTNode*)node;

			bool isReachable = true;

			// Every branch must have a return statement
			isReachable &= __Analyser_isReturnReachable(analyser, ifStatement->body);

			if(ifStatement->alternate) {
				if(ifStatement->alternate->_type == NODE_BLOCK) {
					isReachable &= __Analyser_isReturnReachable(analyser, (BlockASTNode*)ifStatement->alternate);
				} else {
					isReachable &= __Analyser_isReturnReachable_processNode(analyser, ifStatement->alternate);
				}
			} else {
				isReachable = false;
			}

			return isReachable;
		} break;

		default: return false;
	}
}

bool __Analyser_isReturnReachable(Analyser *analyser, BlockASTNode *block) {
	for(size_t i = 0; i < block->statements->size; i++) {
		StatementASTNode *statement = Array_get(block->statements, i);

		if(__Analyser_isReturnReachable_processNode(analyser, statement)) return true;

		// TODO: Cut off unreachable code here
	}

	return false;
}

AnalyserResult __Analyser_resolveFunctionOverloadCandidates(
	Analyser *analyser,
	FunctionCallASTNode *node,
	BlockScope *scope,
	Array /*<FunctionDeclaration>*/ **outCandidates
) {
	Array *overloads = HashMap_get(analyser->overloads, node->id->name->value);
	if(!overloads) {
		return AnalyserError(
			RESULT_ERROR_SEMANTIC_UNDEFINED_FUNCTION,
			String_fromFormat("cannot find function '%s' in scope", node->id->name->value),
			NULL
		);
	}

	Array *candidates = Array_alloc(0);

	// Loop over possible candidates
	for(size_t i = 0; i < overloads->size; i++) {
		FunctionDeclaration *overload = Array_get(overloads, i);

		Array *parameters = overload->node->parameterList->parameters;
		Array *arguments = node->argumentList->arguments;

		// Non-matching parameter count
		if(parameters->size != arguments->size) continue;

		// Loop over parameters
		bool hasMatched = true;
		for(size_t j = 0; j < parameters->size; j++) {
			ParameterASTNode *parameter = Array_get(parameters, j);
			ArgumentASTNode *argument = Array_get(arguments, j);

			ValueType parameterType = parameter->type->type;
			ValueType argumentType;

			// Check for label
			String *externalName = parameter->externalId ? parameter->externalId->name : parameter->internalId->name;
			assertf(externalName, "Parameter has no external or internal name");
			if(parameter->isLabeless && argument->label) {
				hasMatched = false;
				break;
			}
			if(!parameter->isLabeless && !argument->label) {
				hasMatched = false;
				break;
			}
			if(!parameter->isLabeless && !String_equals(externalName, argument->label->name->value)) {
				hasMatched = false;
				break;
			}

			AnalyserResult result = Analyser_resolveExpressionType(analyser, argument->expression, scope, parameterType, &argumentType);
			if(!result.success) {
				Array_free(candidates);
				return result;
			}

			// Non-matching parameter type
			// TODO: try casting an argument to the parameter type
			if(!is_value_assignable(parameterType, argumentType)) {
				hasMatched = false;
				break;
			}
		}

		if(hasMatched) Array_push(candidates, overload);
	}

	*outCandidates = candidates;

	return AnalyserSuccess();
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

	// Register built-in functions
	__Analyser_registerBuiltInFunctions(analyser);

	__Analyser_createBlockScopeChaining(analyser, ast->block, NULL);
	analyser->globalScope = ast->block->scope;

	AnalyserResult result = __Analyser_collectFunctionDeclarations(analyser);
	if(!result.success) return result;

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

String* __Analyser_formatBooleanTestErrorMessage(ValueType type) {
	return String_fromFormat(
		"type '%s' cannot be used as a boolean%s",
		__Analyser_stringifyType(type)->value,
		type.isNullable ? "; test for '= nil' instead" :
			type.type == TYPE_INT ? "; test for '!= 0' instead" :
				type.type == TYPE_DOUBLE ? "; test for '!= 0.0' instead" :
					type.type == TYPE_STRING ? "; test for '!= \"\"' instead" :
						""
	);
}

void __Analyser_registerBuiltInFunctions(Analyser *analyser) {
	assertf(analyser->ast != NULL);

	Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	Lexer_setSource(
		&lexer,
		#define LF "\n"
		"func readString() -> String? {return nil}" LF
		"func readInt() -> Int? {return nil}" LF
		"func readDouble() -> Double? {return nil}" LF
		"func write() {}" LF // Parameters handled internally
		"func Int2Double(_ term: Int) -> Double {return 0.0}" LF
		"func Double2Int(_ term: Double) -> Int {return 0}" LF
		"func length(_ s: String) -> Int {return 0}" LF
		"func substring(of s: String, startingAt i: Int, endingBefore j: Int) -> String? {return nil}" LF
		"func ord(_ c: String) -> Int {return 0}" LF
		"func chr(_ i: Int) -> String {return \"\"}" LF
		#undef LF
	);
	ParserResult result = Parser_parse(&parser);
	assertf(result.success, "Failed to parse built-in function declarations: %s", result.message->value);

	ProgramASTNode *ast = (ProgramASTNode*)result.node;
	Array *statements = ast->block->statements;

	for(size_t i = 0; i < FUNCTIONS_COUNT; i++) {
		FunctionDeclarationASTNode *functionNode = (FunctionDeclarationASTNode*)Array_get(statements, i);

		functionNode->builtin = (enum BuiltInFunction)i;

		Array_unshift(analyser->ast->block->statements, functionNode);
	}
}

BlockScope* __Analyser_createBlockScopeChaining(Analyser *analyser, BlockASTNode *block, BlockScope *parent) {
	block->scope = BlockScope_alloc(parent);

	for(size_t i = 0; i < block->statements->size; i++) {
		StatementASTNode *statement = Array_get(block->statements, i);

		__Analyser_createBlockScopeChaining_processNode(analyser, statement, block->scope);
	}

	return block->scope;
}

void __Analyser_createBlockScopeChaining_processNode(Analyser *analyser, ASTNode *node, BlockScope *parent) {
	// if(node->_type != NODE_BLOCK) return;

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

		case NODE_WHILE_STATEMENT:
		case NODE_FOR_STATEMENT: {
			WhileStatementASTNode *loopStatement = (WhileStatementASTNode*)node;
			BlockScope *child = __Analyser_createBlockScopeChaining(analyser, loopStatement->body, parent);
			child->loop = (StatementASTNode*)loopStatement;
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

AnalyserResult __Analyser_validateTestCondition(Analyser *analyser, ASTNode *node, BlockScope *scope) {
	assertf(node->_type == NODE_IF_STATEMENT || node->_type == NODE_WHILE_STATEMENT, "Invalid node type %d", node->_type);

	IfStatementASTNode *conditionalStatemnt = (IfStatementASTNode*)node;

	if(conditionalStatemnt->test->_type == NODE_OPTIONAL_BINDING_CONDITION) {
		OptionalBindingConditionASTNode *condition = (OptionalBindingConditionASTNode*)conditionalStatemnt->test;
		IdentifierASTNode *identifier = condition->id;

		VariableDeclaration *declaration = Analyser_getVariableByName(analyser, identifier->name->value, scope);

		// Variable is not declared in reachable scopes
		if(!declaration) {
			return AnalyserError(
				RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE,
				String_fromFormat("cannot find '%s' in scope", identifier->name->value),
				NULL
			);
		}

		// Validate type of the variable
		if(!declaration->type.isNullable) {
			return AnalyserError(
				RESULT_ERROR_SEMANTIC_OTHER,
				String_fromFormat(
					"initializer for conditional binding must have Optional type, not '%s'",
					__Analyser_stringifyType(declaration->type)->value
				),
				NULL
			);
		}

		// Create a new declaration for the variable
		VariableDeclaration *newDeclaration = new_VariableDeclaration(
			analyser,
			NULL,
			true,
			(ValueType){.type = declaration->type.type, .isNullable = false},
			declaration->name,
			false,
			true
		);

		// Add the new declaration to the scope of the while statement body
		HashMap_set(conditionalStatemnt->body->scope->variables, newDeclaration->name->value, newDeclaration);

		// Set the id of the identifier node to the id of the new declaration
		identifier->id = newDeclaration->id;

		// Set the id from which the value is unwrapped
		condition->fromId = declaration->id;
	} else {
		// Get the type of the test expression
		ValueType type;
		AnalyserResult result = Analyser_resolveExpressionType(analyser, conditionalStatemnt->test, scope, (ValueType){.type = TYPE_BOOL, .isNullable = false}, &type);
		if(!result.success) return result;

		// Validate the type of the test expression
		if(type.type != TYPE_BOOL) {
			return AnalyserError(
				RESULT_ERROR_SEMANTIC_INVALID_TYPE,
				__Analyser_formatBooleanTestErrorMessage(type),
				NULL
			);
		}
	}

	return AnalyserSuccess();
}

AnalyserResult __Analyser_analyseBlock(Analyser *analyser, BlockASTNode *block) {
	for(size_t i = 0; i < block->statements->size; i++) {
		StatementASTNode *statement = Array_get(block->statements, i);

		switch(statement->_type) {
			case NODE_VARIABLE_DECLARATION: {
				VariableDeclarationASTNode *declarationNode = (VariableDeclarationASTNode*)statement;
				Array *declarators = declarationNode->declaratorList->declarators;

				// Try to find the nearest function scope
				FunctionDeclaration *function = Analyser_getNearestFunctionDeclaration(analyser, block->scope);

				for(size_t j = 0; j < declarators->size; j++) {
					VariableDeclaratorASTNode *declaratorNode = Array_get(declarators, j);

					// Cannot resolve type (provided type is not supported)
					if(declaratorNode->pattern->type) {
						declaratorNode->pattern->type->type.type = Analyser_resolveBuiltInType(declaratorNode->pattern->type->id->name);
						declaratorNode->pattern->type->type.isNullable = declaratorNode->pattern->type->isNullable;

						if(declaratorNode->pattern->type->type.type == TYPE_INVALID) {
							// return AnalyserError(
							// 	RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							// 	String_fromFormat("cannot find type '%s' in scope", declaratorNode->pattern->type->id->name->value),
							// 	NULL
							// );
							return AnalyserError(
								RESULT_ERROR_SYNTACTIC_ANALYSIS,
								String_fromFormat("cannot find type '%s' in scope", declaratorNode->pattern->type->id->name->value),
								NULL
							);
						}
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
						ValueType type;
						AnalyserResult result = Analyser_resolveExpressionType(analyser, declaratorNode->initializer, block->scope, declaration->type, &type);
						if(!result.success) return result;

						// This was requested by the assignment
						if(declaration->type.type == TYPE_VOID) {
							return AnalyserError(
								RESULT_ERROR_SEMANTIC_INVALID_TYPE,
								String_fromFormat("cannot use initializer for variable of type 'Void'"),
								NULL
							);
						}

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
							if(type.type == TYPE_NIL) {
								return AnalyserError(
									RESULT_ERROR_SEMANTIC_FAILED_INFER,
									String_fromFormat("'nil' requires a contextual type"),
									NULL
								);
							}

							declaration->type = type;
						}
					}

					// null-initialize the non-initialized nullable variables
					if(declaration->type.isNullable && !declaration->isInitialized) {
						declaratorNode->initializer = (ExpressionASTNode*)new_LiteralExpressionASTNode(
							(ValueType){.type = TYPE_NIL, .isNullable = true},
							(union TokenValue){0}
						);
						declaration->isInitialized = true;
					}

					// Look for already existing variable with the same
					VariableDeclaration *existingDeclaration = HashMap_get(block->scope->variables, declaration->name->value);

					// There is already a variable with the same name in the current scope
					if(existingDeclaration && existingDeclaration->isUserDefined) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_VARIABLE_REDEFINITION, // TODO: Fixed
							String_fromFormat("invalid redeclaration of '%s'", declaration->name->value),
							NULL
						);
					}

					// If the variable is in global scope together with a function declaration, it is an error
					if(!block->scope->parent) {
						Array *functions = Analyser_getFunctionDeclarationsByName(analyser, declaration->name->value);

						// There has to be at least one function declaration with no parameters
						if(functions && functions->size > 0) {
							for(size_t i = 0; i < functions->size; i++) {
								FunctionDeclaration *function = Array_get(functions, i);

								if(function->node->parameterList->parameters->size == 0) {
									return AnalyserError(
										RESULT_ERROR_SEMANTIC_VARIABLE_REDEFINITION, // TODO: Fixed
										String_fromFormat("invalid redeclaration of '%s'", declaration->name->value),
										NULL
									);
								}
							}
						}
					}

					// Add the variable declaration to the current scope
					HashMap_set(block->scope->variables, declaration->name->value, declaration);

					// Register the variable declaration to the global/function scope
					String *id = String_fromLong(declaration->id);

					if(function) {
						HashMap_set(function->variables, id->value, declaration);
					} else {
						HashMap_set(analyser->variables, id->value, declaration);
					}
				}
			} break;

			case NODE_ASSIGNMENT_STATEMENT: {
				AssignmentStatementASTNode *assignment = (AssignmentStatementASTNode*)statement;
				VariableDeclaration *variable = Analyser_getVariableByName(analyser, assignment->id->name->value, block->scope);

				// Variable is not declared in reachable scopes
				if(!variable) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE,
						String_fromFormat("cannot find '%s' in scope", assignment->id->name->value),
						NULL
					);
				}

				// Constant variable is already initialized
				if(variable->isConstant && variable->isInitialized) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
						String_fromFormat("cannot assign to constant '%s'", assignment->id->name->value),
						NULL
					);
				}

				ValueType type;
				AnalyserResult result = Analyser_resolveExpressionType(analyser, assignment->expression, block->scope, variable->type, &type);
				if(!result.success) return result;

				if(!is_value_assignable(variable->type, type)) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_TYPE, // TODO: Fixed
						String_fromFormat(
							"cannot convert value of type '%s' to specified type '%s'",
							__Analyser_stringifyType(type)->value,
							__Analyser_stringifyType(variable->type)->value
						),
						NULL
					);
				}

				if(HashMap_has(block->scope->variables, variable->name->value)) variable->isInitialized = true; // This is too much strict
				variable->isUsed = true;
				assignment->id->id = variable->id;
			} break;

			case NODE_IF_STATEMENT: {
				IfStatementASTNode *ifStatement = (IfStatementASTNode*)statement;

				while(ifStatement->_type == NODE_IF_STATEMENT) {
					// Resolve the condition of the if statement
					AnalyserResult result = __Analyser_validateTestCondition(analyser, (ASTNode*)ifStatement, block->scope);
					if(!result.success) return result;

					// Assign the id to the if statement for the codegen
					ifStatement->id = Analyser_nextId(analyser);

					// Analyse the body of the if statement
					result = __Analyser_analyseBlock(analyser, ifStatement->body);
					if(!result.success) return result;

					// If the if statement has no alternate, we are done
					if(!ifStatement->alternate) break;

					// Process the alternate
					if(ifStatement->alternate->_type == NODE_BLOCK) {
						result = __Analyser_analyseBlock(analyser, (BlockASTNode*)ifStatement->alternate);
						if(!result.success) return result;

						// There cannot be another if statement after the alternate, so we are done
						break;
					} else {
						ifStatement = (IfStatementASTNode*)ifStatement->alternate;
					}
				}
			} break;

			case NODE_WHILE_STATEMENT: {
				WhileStatementASTNode *whileStatement = (WhileStatementASTNode*)statement;

				// Resolve the condition of the while statement
				AnalyserResult result = __Analyser_validateTestCondition(analyser, (ASTNode*)whileStatement, block->scope);
				if(!result.success) return result;

				// Assign the id to the while statement for the codegen
				whileStatement->id = Analyser_nextId(analyser);

				// Analyse the body of the while statement
				result = __Analyser_analyseBlock(analyser, whileStatement->body);
				if(!result.success) return result;
			} break;

			case NODE_FOR_STATEMENT: {
				ForStatementASTNode *forStatement = (ForStatementASTNode*)statement;
				RangeASTNode *range = forStatement->range;
				assertf(range != NULL);

				// Resolve types for the range
				ValueType startType;
				AnalyserResult result = Analyser_resolveExpressionType(analyser, range->start, block->scope, (ValueType){.type = TYPE_INT, .isNullable = false}, &startType);
				if(!result.success) return result;

				ValueType endType;
				result = Analyser_resolveExpressionType(analyser, range->end, block->scope, (ValueType){.type = TYPE_INT, .isNullable = false}, &endType);
				if(!result.success) return result;

				// Check if the types are assignable
				ValueType allowedType = (ValueType){.type = TYPE_INT, .isNullable = false};
				bool isStart = is_value_assignable(allowedType, startType);
				bool isEnd = is_value_assignable(allowedType, endType);
				if(!isStart || !isEnd) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_TYPE,
						String_fromFormat(
							"cannot convert value of type '%s' to specified type '%s'",
							__Analyser_stringifyType(isStart ? startType : endType)->value,
							__Analyser_stringifyType(allowedType)->value
						),
						NULL
					);
				}

				// Create a new declaration for the loop variable
				VariableDeclaration *declaration = new_VariableDeclaration(
					analyser,
					NULL,
					true,
					(ValueType){.type = TYPE_INT, .isNullable = false},
					forStatement->iterator->name,
					false,
					true
				);

				// Add the new declaration to the scope of the for statement body
				HashMap_set(forStatement->body->scope->variables, declaration->name->value, declaration);

				// Assign the id of the iterator to the for statement for the codegen
				forStatement->iterator->id = declaration->id;

				// Assign the id to the for statement for the codegen
				forStatement->id = Analyser_nextId(analyser);

				// Analyse the body of the for statement
				result = __Analyser_analyseBlock(analyser, forStatement->body);
				if(!result.success) return result;
			} break;

			case NODE_FUNCTION_DECLARATION: {
				FunctionDeclarationASTNode *function = (FunctionDeclarationASTNode*)statement;

				AnalyserResult result = __Analyser_analyseBlock(analyser, function->body);
				if(!result.success) return result;

				FunctionDeclaration *declaration = Analyser_getFunctionById(analyser, function->id->id);
				assertf(declaration != NULL, "Cannot find function declaration with id %ld", function->id->id);

				// TODO: handle implicit return

				// If there is a variable in the global scope with the same name, it is an error
				if(declaration->node->parameterList->parameters->size == 0) {
					VariableDeclaration *variable = Analyser_getVariableByName(analyser, declaration->node->id->name->value, block->scope);

					if(variable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_VARIABLE_REDEFINITION, // TODO: Fixed
							String_fromFormat("invalid redeclaration of '%s'", declaration->node->id->name->value),
							NULL
						);
					}
				}

				// Analyse the return statement reachability
				if(declaration->returnType.type != TYPE_VOID && !__Analyser_isReturnReachable(analyser, function->body)) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_RETURN,
						String_fromFormat(
							"missing return in global function expected to return '%s'",
							__Analyser_stringifyType(declaration->returnType)->value
						),
						NULL
					);
				}
			} break;

			case NODE_RETURN_STATEMENT: {
				ReturnStatementASTNode *returnStatement = (ReturnStatementASTNode*)statement;

				// Try to find the nearest function scope
				FunctionDeclaration *function = Analyser_getNearestFunctionDeclaration(analyser, block->scope);

				// Return statement is not inside a function
				if(!function) {
					return AnalyserError(
						RESULT_ERROR_SYNTACTIC_ANALYSIS, // This was requested by the examiner
						String_fromFormat("return invalid outside of a func"),
						NULL
					);
				}

				// Function has a return type but returns nothing
				if(function->returnType.type != TYPE_VOID && !returnStatement->expression) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_RETURN,
						String_fromFormat("non-void function should return a value"),
						NULL
					);
				}

				// Function has a return type but returns incompatible type
				if(/*function->returnType.type != TYPE_VOID && */ returnStatement->expression) {
					// Get the type of the return expression
					ValueType type;
					AnalyserResult result = Analyser_resolveExpressionType(analyser, returnStatement->expression, block->scope, function->returnType, &type);
					if(!result.success) return result;

					// Validate the type of the return expression
					if(!is_value_assignable(function->returnType, type)) {
						if(function->returnType.type == TYPE_VOID) {
							return AnalyserError(
								RESULT_ERROR_SEMANTIC_INVALID_RETURN,
								String_fromFormat("unexpected non-void return value in void function"),
								NULL
							);
						}

						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_RETURN_TYPE,
							String_fromFormat(
								"cannot convert value of type '%s' to specified type '%s'",
								__Analyser_stringifyType(type)->value,
								__Analyser_stringifyType(function->returnType)->value
							),
							NULL
						);
					}
				}

				// Assign the id of the function to the return statement for the codegen
				returnStatement->id = function->id;

				function->isUsed = true;
			} break;

			case NODE_BREAK_STATEMENT:
			case NODE_CONTINUE_STATEMENT: {
				BreakStatementASTNode *breakStatement = (BreakStatementASTNode*)statement;

				// Try to find the nearest loop
				WhileStatementASTNode *loop = (WhileStatementASTNode*)Analyser_getNearestLoop(analyser, block->scope);

				// Loop control statement is not inside a function
				if(!loop) {
					return AnalyserError(
						RESULT_ERROR_SYNTACTIC_ANALYSIS, // This was requested by the examiner
						String_fromFormat(
							"'%s' is only allowed inside a loop",
							breakStatement->_type == NODE_BREAK_STATEMENT ? "break" : "continue"
						),
						NULL
					);
				}

				// Assign the id of the function to the return statement for the codegen
				breakStatement->id = loop->id;
			} break;

			case NODE_EXPRESSION_STATEMENT: {
				ExpressionStatementASTNode *expressionStatement = (ExpressionStatementASTNode*)statement;

				ValueType type;
				AnalyserResult result = Analyser_resolveExpressionType(
					analyser,
					expressionStatement->expression,
					block->scope,
					(ValueType){.type = TYPE_UNKNOWN, .isNullable = false},
					&type
				);
				if(!result.success) return result;
			} break;

			default: {
				warnf("TODO: Analyse statement %d", statement->_type);
			} break;
		}

	}

	// TODO: Uncomment this to print out all unhandled nodes
	return AnalyserSuccess();
}

AnalyserResult Analyser_resolveExpressionType(Analyser *analyser, ExpressionASTNode *node, BlockScope *scope, ValueType prefferedType, ValueType *outType) {
	switch(node->_type) {
		case NODE_LITERAL_EXPRESSION: {
			// Literals are handled by parser, we just might need to retype them if requested
			LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)node;

			// Can only retype int literals to double
			if(prefferedType.type == TYPE_DOUBLE && literal->originalType.type == TYPE_INT) {
				literal->value.floating = (double)literal->originalValue.integer;
				literal->type.type = TYPE_DOUBLE;
			}
			// Revert implicit type conversions
			else if(
				/*prefferedType.type == TYPE_INT && literal->originalType.type == TYPE_INT*/
				literal->originalType.type == TYPE_INT && literal->type.type != TYPE_DOUBLE
			) {
				literal->value = literal->originalValue;
				literal->type = literal->originalType;
			}

			*outType = literal->type;
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

			VariableDeclaration *declaration = Analyser_getVariableByName(analyser, identifier->name->value, scope);

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

			if(!declaration->type.isNullable && !declaration->isInitialized) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE,
					String_fromFormat("variable '%s' used before being initialized", identifier->name->value),
					NULL
				);
			}

			identifier->id = declaration->id;
			declaration->isUsed = true;
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

			// If not in global scope, look for non-global variable declaration
			if(scope->parent) {
				VariableDeclaration *declaration = Analyser_getVariableByName(analyser, call->id->name->value, scope);

				if(declaration) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL_TYPE,
						String_fromFormat(
							"cannot call value of non-function type '%s'",
							__Analyser_stringifyType(declaration->type)->value
						),
						NULL
					);
				}
			}

			Array /*<FunctionDeclaration> | null*/ *overloads = Analyser_getFunctionDeclarationsByName(analyser, call->id->name->value);
			Array /*<FunctionDeclaration> | null*/ *candidates = NULL;
			bool hasMultipleCandidates = false;
			FunctionDeclaration *declaration = NULL;

			if(overloads && overloads->size > 1) {
				AnalyserResult result = __Analyser_resolveFunctionOverloadCandidates(analyser, call, scope, &candidates);
				if(!result.success) return result;

				// Try to find the candidate with preffered return type
				for(size_t i = 0; i < candidates->size; i++) {
					FunctionDeclaration *candidate = Array_get(candidates, i);

					if(prefferedType.type == TYPE_UNKNOWN || is_type_equal(prefferedType, candidate->returnType) || is_value_assignable(prefferedType, candidate->returnType)) {
						// Multiple candidates matching
						if(declaration) {
							hasMultipleCandidates = true;
							break;
						}

						declaration = candidate;
					}
				}
			} else if(overloads) {
				declaration = Array_get(overloads, 0);
				assertf(declaration);
			} else {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_UNDEFINED_FUNCTION,
					String_fromFormat("cannot find '%s' in scope", call->id->name->value),
					NULL
				);
			}

			// Handle built-in 'write' function differently
			if((!declaration || (overloads && overloads->size == 1)) && String_equals(call->id->name, "write")) {
				Array /*<FunctionDeclaration>*/ *writeCandidates = Analyser_getFunctionDeclarationsByName(analyser, "write");
				assertf(writeCandidates && writeCandidates->size > 0, "Cannot find built-in 'write' function");

				// First function should be the built-in 'write' function
				FunctionDeclaration *writeFunc = Array_get(writeCandidates, 0);
				assertf(writeFunc);

				if(writeFunc->node->builtin == FUNCTION_WRITE) {
					declaration = writeFunc;

					// Resolve all the arguments
					Array /*<ArgumentASTNode>*/ *arguments = call->argumentList->arguments;

					for(size_t i = 0; i < arguments->size; i++) {
						ArgumentASTNode *argument = Array_get(arguments, i);

						ValueType type;
						AnalyserResult result = Analyser_resolveExpressionType(analyser, argument->expression, scope, (ValueType){.type = TYPE_UNKNOWN, .isNullable = false}, &type);
						if(!result.success) return result;

						if(!is_type_valid(type.type) || type.type == TYPE_VOID) {
							return AnalyserError(
								RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL_TYPE,
								String_fromFormat(
									"cannot convert value of type '%s' to expected argument type 'Int? | Double? | String? | Bool?'",
									__Analyser_stringifyType(type)->value
								),
								NULL
							);
						}
					}

					// Set properties of the call node
					call->id->id = declaration->id;
					declaration->isUsed = true;
					*outType = declaration->returnType;

					return AnalyserSuccess();
				}
			}

			if(candidates) {
				Array_free(candidates);
				candidates = NULL;
			}

			if(!declaration) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_UNDEFINED_FUNCTION,
					String_fromFormat("no exact matches in call to global function '%s'", call->id->name->value),
					NULL
				);
			}

			if(hasMultipleCandidates) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
					String_fromFormat("ambiguous use of '%s'", call->id->name->value),
					NULL
				);
			}

			// Check for correct label names
			Array /*<ArgumentASTNode>*/ *arguments = call->argumentList->arguments;
			Array /*<ParameterASTNode>*/ *parameters = declaration->node->parameterList->parameters;
			size_t length = max(arguments->size, parameters->size);

			for(size_t i = 0; i < length; i++) {
				ArgumentASTNode *argument = Array_get(arguments, i);
				ParameterASTNode *parameter = Array_get(parameters, i);

				// Missing argument for parameter
				if(!argument) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL_TYPE,
						String_fromFormat("missing argument for parameter '%s' in call", parameter->externalId->name->value),
						NULL
					);
				}

				// Too many arguments for parameters passed
				if(!parameter) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL_TYPE,
						String_fromFormat("extra argument in call"),
						NULL
					);
				}

				String *externalName = parameter->externalId ? parameter->externalId->name : parameter->internalId->name;
				assertf(externalName, "Parameter has no external or internal name");

				// Parameter is labeless, but argument has a label
				if(parameter->isLabeless && argument->label) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
						String_fromFormat("extraneous argument label '%s' in call", argument->label->name->value),
						NULL
					);
				}

				// Parameter has a label, but argument has no label
				if(!parameter->isLabeless && !argument->label) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
						String_fromFormat("missing argument label '%s' in call", externalName->value),
						NULL
					);
				}

				// Parameter has a label, but argument has a different label
				if(!parameter->isLabeless && argument->label && !String_equals(externalName, argument->label->name->value)) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
						String_fromFormat(
							"incorrect argument label in call (have '%s', expected '%s')",
							argument->label->name->value,
							externalName->value
						),
						NULL
					);
				}

				// TODO: Might need to retype all the parameters to match the overload
				ValueType type;
				AnalyserResult result = Analyser_resolveExpressionType(analyser, argument->expression, scope, parameter->type->type, &type);
				if(!result.success) return result;

				// Is this really needed? Not sure so keeping it here
				if(!is_value_assignable(parameter->type->type, type)) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL_TYPE,
						String_fromFormat(
							"cannot convert value of type '%s' to expected argument type '%s'",
							__Analyser_stringifyType(type)->value,
							__Analyser_stringifyType(parameter->type->type)->value
						),
						NULL
					);
				}
			}

			call->id->id = declaration->id;
			declaration->isUsed = true;
			*outType = declaration->returnType;

			return AnalyserSuccess();
		} break;

		case NODE_UNARY_EXPRESSION: {
			UnaryExpressionASTNode *unary = (UnaryExpressionASTNode*)node;

			switch(unary->operator) {
				case OPERATOR_UNWRAP: {
					ValueType type;
					AnalyserResult result = Analyser_resolveExpressionType(analyser, unary->argument, scope, (ValueType){.type = prefferedType.type, .isNullable = true}, &type);
					if(!result.success) return result;

					if(!type.isNullable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE, // TODO ?
							String_fromFormat("cannot force unwrap value of non-optional type '%s'", __Analyser_stringifyType(type)->value),
							NULL
						);
					}

					unary->type = (ValueType){.type = type.type, .isNullable = false};
				} break;

				case OPERATOR_NOT: {
					ValueType type;
					AnalyserResult result = Analyser_resolveExpressionType(analyser, unary->argument, scope, prefferedType, &type);
					if(!result.success) return result;

					if(type.type != TYPE_BOOL) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							__Analyser_formatBooleanTestErrorMessage(type),
							NULL
						);
					}

					if(type.isNullable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"value of optional type '%s' must be unwrapped to a value of type 'Bool'",
								__Analyser_stringifyType(type)->value
							),
							NULL
						);
					}

					unary->type = (ValueType){.type = TYPE_BOOL, .isNullable = false};
				} break;

				default: {
					return AnalyserError(
						RESULT_ERROR_SYNTACTIC_ANALYSIS,
						String_fromFormat(
							"'%s' is not a %s unary operator",
							__Analyser_stringifyOperator(unary->operator),
							unary->isPrefix ? "prefix" : "postfix"
						),
						NULL
					);
				} break;
			}

			*outType = unary->type;
		} break;

		case NODE_BINARY_EXPRESSION: {
			BinaryExpressionASTNode *binary = (BinaryExpressionASTNode*)node;

			AnalyserResult result;
			ValueType leftType;
			ValueType rightType;

			if(prefferedType.type != TYPE_UNKNOWN) {
				result = Analyser_resolveExpressionType(analyser, binary->left, scope, prefferedType, &leftType);
				if(!result.success) return result;

				result = Analyser_resolveExpressionType(analyser, binary->right, scope, prefferedType, &rightType);
				if(!result.success) return result;
			} else {
				// Try to correctly suggest function overload candidates
				bool isLeftFunctionCall = binary->left->_type == NODE_FUNCTION_CALL;
				bool isRightFunctionCall = binary->right->_type == NODE_FUNCTION_CALL;

				// No function call
				if(!isLeftFunctionCall && !isRightFunctionCall) {
					// Resolve both types independently
					AnalyserResult resultLeft = Analyser_resolveExpressionType(analyser, binary->left, scope, prefferedType, &leftType);
					AnalyserResult resultRight = Analyser_resolveExpressionType(analyser, binary->right, scope, prefferedType, &rightType);

					// Both sides failed, return the error from the left side
					if(!resultLeft.success && !resultRight.success) return resultLeft;

					// Both sides resolved successfully
					if(resultLeft.success && resultRight.success) {
						// Left is int, right is double
						if(leftType.type == TYPE_INT && rightType.type == TYPE_DOUBLE) {
							// Try to convert the left side to double
							result = Analyser_resolveExpressionType(analyser, binary->left, scope, rightType, &leftType);
							if(!result.success) return result;
						}
						// Left is double, right is int
						else if(leftType.type == TYPE_DOUBLE && rightType.type == TYPE_INT) {
							// Try to convert the right side to double
							result = Analyser_resolveExpressionType(analyser, binary->right, scope, leftType, &rightType);
							if(!result.success) return result;
						}
						// Both types are same, we are done
						else {
							// Type casts resolved successfully, yay!
						}
					}
					// Right side failed, try to resolve it with left side type
					else if(resultLeft.success) {
						result = Analyser_resolveExpressionType(analyser, binary->right, scope, leftType, &rightType);

						// Try to convert the right side to double
						if(!result.success && leftType.type == TYPE_INT) {
							// Try to resolve the left side with double
							result = Analyser_resolveExpressionType(analyser, binary->left, scope, (ValueType){.type = TYPE_DOUBLE, .isNullable = leftType.isNullable}, &leftType);
							if(result.success && leftType.type == TYPE_DOUBLE) {
								// Try to resolve the right side with the left side type
								result = Analyser_resolveExpressionType(analyser, binary->right, scope, leftType, &rightType);
								if(!result.success) return result; // If this fails, there is nothing we can do
							} else {
								return resultRight; // If this fails, there is nothing we can do
							}
						} else if(!result.success) {
							return resultRight; // If this fails, there is nothing we can do
						} else {
							// All resolved successfully, yay!
						}
					}
					// Left side failed, try to resolve it with right side type
					else if(resultRight.success) {
						result = Analyser_resolveExpressionType(analyser, binary->left, scope, rightType, &leftType);

						// Try to convert the left side to double
						if(!result.success && rightType.type == TYPE_INT) {
							// Try to resolve the right side with double
							result = Analyser_resolveExpressionType(analyser, binary->right, scope, (ValueType){.type = TYPE_DOUBLE, .isNullable = rightType.isNullable}, &rightType);
							if(result.success && rightType.type == TYPE_DOUBLE) {
								// Try to resolve the left side with the right side type
								result = Analyser_resolveExpressionType(analyser, binary->left, scope, rightType, &leftType);
								if(!result.success) return result; // If this fails, there is nothing we can do
							} else {
								return resultLeft; // If this fails, there is nothing we can do
							}
						} else if(!result.success) {
							return resultLeft; // If this fails, there is nothing we can do
						} else {
							// All resolved successfully, yay!
						}
					} else {
						// Both sides failed, return the error from the left side
						return resultLeft;
					}
				}
				// Left first (right function call)
				else if(!isLeftFunctionCall && isRightFunctionCall) {
					result = Analyser_resolveExpressionType(analyser, binary->left, scope, prefferedType, &leftType);
					if(!result.success) return result;

					result = Analyser_resolveExpressionType(analyser, binary->right, scope, leftType, &rightType);
					if(!result.success) return result;
				}
				// Right first (left function call)
				else if(isLeftFunctionCall && !isRightFunctionCall) {
					result = Analyser_resolveExpressionType(analyser, binary->right, scope, prefferedType, &rightType);
					if(!result.success) return result;

					result = Analyser_resolveExpressionType(analyser, binary->left, scope, rightType, &leftType);
					if(!result.success) return result;
				}
				// Left, then right (both function calls)
				else if(isLeftFunctionCall && isRightFunctionCall) {
					// Try to resolve the left side first
					result = Analyser_resolveExpressionType(analyser, binary->left, scope, prefferedType, &leftType);
					if(result.success) {
						result = Analyser_resolveExpressionType(analyser, binary->right, scope, leftType, &rightType);
						if(!result.success) return result;
					} else {
						// Try to resolve the right side after the left side failed
						result = Analyser_resolveExpressionType(analyser, binary->right, scope, prefferedType, &rightType);
						if(result.success) {
							result = Analyser_resolveExpressionType(analyser, binary->left, scope, rightType, &leftType);
							if(!result.success) return result;
						} else {
							// Both sides failed, return the error from the left side
							return result;
						}
					}
				}
			}

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
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
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
						// LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->left;

						// literal->value.floating = (double)literal->value.integer;
						// literal->type.type = TYPE_DOUBLE;

						binary->type.type = TYPE_DOUBLE;
					} else if(
						binary->operator != OPERATOR_DIV &&
						binary->right->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_DOUBLE && rightType.type == TYPE_INT
					) {
						// LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->right;

						// literal->value.floating = (double)literal->value.integer;
						// literal->type.type = TYPE_DOUBLE;

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
					*outType = binary->type;
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
					} else if(leftType.type == TYPE_NIL || rightType.type == TYPE_NIL) {
						binary->type.type = TYPE_BOOL;
					} else if(
						binary->left->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_INT && rightType.type == TYPE_DOUBLE
					) {
						// LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->left;

						// literal->value.floating = (double)literal->value.integer;
						// literal->type.type = TYPE_DOUBLE;

						binary->type.type = TYPE_BOOL;
					} else if(
						binary->right->_type == NODE_LITERAL_EXPRESSION &&
						leftType.type == TYPE_DOUBLE && rightType.type == TYPE_INT
					) {
						// LiteralExpressionASTNode *literal = (LiteralExpressionASTNode*)binary->right;

						// literal->value.floating = (double)literal->value.integer;
						// literal->type.type = TYPE_DOUBLE;

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

					binary->type.isNullable = leftType.isNullable || rightType.isNullable;
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
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"cannot use relational operator '%s' with optional type '%s'",
								__Analyser_stringifyOperator(binary->operator),
								__Analyser_stringifyType(leftType)->value
							),
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
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
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

				case OPERATOR_AND:
				case OPERATOR_OR: {
					if(leftType.type != TYPE_BOOL || rightType.type != TYPE_BOOL) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							__Analyser_formatBooleanTestErrorMessage(leftType.type != TYPE_BOOL ? leftType : rightType),
							NULL
						);
					}

					if(leftType.isNullable || rightType.isNullable) {
						return AnalyserError(
							RESULT_ERROR_SEMANTIC_INVALID_TYPE,
							String_fromFormat(
								"value of optional type '%s' must be unwrapped to a value of type 'Bool'",
								__Analyser_stringifyType(leftType.isNullable ? leftType : rightType)->value
							),
							NULL
						);
					}

					binary->type = (ValueType){.type = TYPE_BOOL, .isNullable = false};
					*outType = binary->type;
				} break;

				default: {
					fassertf("Failed to resolve binary operator: Unexpected operator %d", binary->operator);
				} break;
			}
		} break;

		case NODE_INTERPOLATION_EXPRESSION: {
			InterpolationExpressionASTNode *interpolation = (InterpolationExpressionASTNode*)node;

			// Validate all the expressions
			for(size_t i = 0; i < interpolation->expressions->size; i++) {
				ExpressionASTNode *expression = Array_get(interpolation->expressions, i);

				ValueType type;
				AnalyserResult result = Analyser_resolveExpressionType(analyser, expression, scope, prefferedType, &type);
				if(!result.success) return result;

				// NOTE: Not specified in the assignment, but Swift is okay with this (just prints some warnings)
				// if(type.isNullable) {
				// 	return AnalyserError(
				// 		RESULT_ERROR_SEMANTIC_INVALID_TYPE,
				// 		String_fromFormat(
				// 			"value of optional type '%s' must be unwrapped to a value of type 'String'",
				// 			__Analyser_stringifyType(type)->value
				// 		),
				// 		NULL
				// 	);
				// }
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

		// Mark block scope as function scope
		declarationNode->body->scope->function = declaration;

		// Register the function declaration to the global scope
		HashMap_set(analyser->functions, String_fromLong(declaration->id)->value, declaration);

		// Add id to the function declaration node
		declarationNode->id->id = declaration->id;

		// Resolve return type
		if(declarationNode->returnType) {
			declaration->returnType.type = Analyser_resolveBuiltInType(declarationNode->returnType->id->name);
			declaration->returnType.isNullable = declarationNode->returnType->isNullable;

			if(!is_type_valid(declaration->returnType.type)) {
				return AnalyserError(
					RESULT_ERROR_SYNTACTIC_ANALYSIS, // TODO: This should be everywhere, when checking types
					String_fromFormat("cannot find type '%s' in scope", declarationNode->returnType->id->name->value),
					NULL
				);
				// return AnalyserError(
				// 	RESULT_ERROR_SEMANTIC_OTHER,
				// 	String_fromFormat("cannot find type '%s' in scope", declarationNode->returnType->id->name->value),
				// 	NULL
				// );
			}
		} else {
			declaration->returnType = (ValueType){.type = TYPE_VOID, .isNullable = false};
		}

		// TODO: Analyse function declaration
		// Check for duplicate parameters
		Array *parameterList = declarationNode->parameterList->parameters;
		{
			HashMap *variables = declaration->node->body->scope->variables;

			// This will effectively check for duplicate parameters + register them as local variables
			for(size_t i = 0; i < parameterList->size; i++) {
				ParameterASTNode *parameter = Array_get(parameterList, i);
				String *name = parameter->internalId->name;

				if(!parameter->type) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
						String_fromFormat("type annotation missing in parameter '%s'", name->value),
						NULL
					);
				}

				// External paramter is missing (this is required in the assignment)
				if(!parameter->externalId) {
					return AnalyserError(
						RESULT_ERROR_SYNTACTIC_ANALYSIS,
						String_fromFormat("external parameter name missing in parameter '%s'", name->value),
						NULL
					);
				}

				// Both name and label are the same
				if(parameter->externalId && String_equals(name, parameter->externalId->name->value)) {
					return AnalyserError(
						RESULT_ERROR_SEMANTIC_OTHER, // TODO: Fixed
						String_fromFormat("parameter name same as external label '%s'", name->value),
						NULL
					);
				}

				String *typeName = parameter->type->id->name;
				enum BuiltInTypes resolvedType = Analyser_resolveBuiltInType(typeName);

				if(!is_type_valid(resolvedType)) {
					return AnalyserError(
						RESULT_ERROR_SYNTACTIC_ANALYSIS, // TODO: Fixed
						String_fromFormat("cannot find type '%s' in scope", typeName->value),
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

				// Update the parameter with resolved type and id
				parameter->type->type = variable->type;
				parameter->internalId->id = variable->id;

				// If the parameter is not in the hashmap yet
				if(!HashMap_has(variables, name->value)) {
					// Add the parameter to the hashmap
					HashMap_set(variables, name->value, variable);
					continue;
				}

				// There is already a parameter with the same name
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_VARIABLE_REDEFINITION, // TODO: Fixed?
					String_fromFormat("invalid redeclaration of '%s'", name->value),
					NULL
				);
			}
		}

		// Query the hashmap for the function declaration
		String *name = declarationNode->id->name;
		Array *overloads = HashMap_get(analyser->overloads, name->value);

		// No overloads yet, create a new array and add the function declaration
		if(!overloads) {
			// Allocate a new array to store possible overloads
			overloads = Array_alloc(1);

			// Add the array to the hashmap
			HashMap_set(analyser->overloads, name->value, overloads);

			// Add the function declaration to the array
			Array_push(overloads, declaration);

			continue;
		}

		// Look for an overload with the same number of parameters
		for(size_t i = 0; i < overloads->size; i++) {
			FunctionDeclaration *overload = Array_get(overloads, i);
			Array *parameters = overload->node->parameterList->parameters;

			// Different number of parameters, skip
			if(parameters->size != parameterList->size) continue;

			// Different return type, skip
			if(!is_type_equal(overload->returnType, declaration->returnType)) continue;

			// Check for parameters
			bool isMatching = false;
			for(size_t j = 0; j < parameters->size; j++) {
				ParameterASTNode *parameter = Array_get(parameters, j);
				ParameterASTNode *otherParameter = Array_get(parameterList, j);

				// Different parameter types, skip
				if(!is_type_equal(parameter->type->type, otherParameter->type->type)) continue;

				// Different external parameter name, skip
				String *externalName = parameter->externalId ? parameter->externalId->name : parameter->internalId->name;
				String *otherExternalName = otherParameter->externalId ? otherParameter->externalId->name : otherParameter->internalId->name;
				assertf(externalName, "Parameter has no external or internal name");
				assertf(otherExternalName, "Parameter has no external or internal name");

				if(!String_equals(externalName, otherExternalName->value)) continue;

				// Found a matching overload
				isMatching = true;
				break;
			}

			// Found a matching overload
			if(isMatching || parameters->size == 0) {
				return AnalyserError(
					RESULT_ERROR_SEMANTIC_VARIABLE_REDEFINITION, // TODO: Fixed
					String_fromFormat("invalid redeclaration of '%s'", name->value),
					NULL
				);
			}
		}

		// No matching overload found, add the function declaration to the array
		Array_push(overloads, declaration);
	}

	return AnalyserSuccess();
}
