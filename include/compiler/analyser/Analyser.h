/**
 * @file include/compiler/analyser/Analyser.h
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include "internal/Array.h"
#include "internal/String.h"
#include "internal/HashMap.h"
#include "compiler/analyser/AnalyserResult.h"

#ifndef ANALYSER_H
#define ANALYSER_H

#include "compiler/parser/ASTNodes.h"

enum DeclarationType {
	DECLARATION_VARIABLE,
	DECLARATION_FUNCTION
};

typedef struct Declaration {
	enum DeclarationType _type;
	size_t id;
} Declaration;

typedef struct VariableDeclaration {
	enum DeclarationType _type;
	size_t id;
	struct VariableDeclaratorASTNode /* | null*/ *node; // null if isUserDefined = 0
	String *name;
	ValueType type;
	bool isConstant;
	bool isUserDefined;
	bool isUsed;
	bool isInitialized;
} VariableDeclaration;

typedef struct FunctionDeclaration {
	enum DeclarationType _type;
	size_t id;
	struct FunctionDeclarationASTNode *node;
	HashMap /*<id: String, declaration: VariableDeclaration>*/ *variables;
	bool isUsed;
	ValueType returnType;
} FunctionDeclaration;

typedef struct BlockScope {
	struct BlockScope *parent;
	HashMap /*<name: String, declaration: VariableDeclaration>*/ *variables;
	FunctionDeclaration *function; // Defined when this is function body scope
	StatementASTNode /*<loop: ForStatementASTNode | WhileStatementASTNode>*/ *loop; // Defined when this is loop body scope
} BlockScope;

typedef struct Analyser {
	ProgramASTNode *ast;
	BlockScope *globalScope;
	HashMap /*<name: String, declarations: Array<FunctionDeclaration>>*/ *overloads; // Relative to global scope
	HashMap /*<id: String, declaration: FunctionDeclaration>*/ *functions; // Relative to global scope //! Unused
	HashMap /*<id: String, declaration: VariableDeclaration>*/ *variables; // Relative to global scope //! Unused
	HashMap /*<id: String, declaration: Declaration>*/ *idsPool;
	HashMap /*<String, String>*/ *types; // TODO: delete this
	size_t idCounter; // Do NOT directly modify this!
} Analyser;


/* Analyser */
/**
 * Constructs the provided Analyser instance.
 * @param analyser
 */
void Analyser_constructor(Analyser *analyser);

/**
 * Destructs the provided Analyser instance.
 * @param analyser
 */
void Analyser_destructor(Analyser *analyser);

/**
 * Analyzes the provided AST and returns the result.
 * @param analyser
 * @param ast The AST to analyze
 * @return AnalyserResult
 */
AnalyserResult Analyser_analyse(Analyser *analyser, ProgramASTNode *ast);


/**
 * Returns the declaration with the provided id or null if it doesn't exist.
 * @param analyser
 * @param id Id of the declaration
 * @return Declaration*<VariableDeclaration | FunctionDeclaration> | null
 */
Declaration /*<VariableDeclaration | FunctionDeclaration> | null*/* Analyser_getDeclarationById(Analyser *analyser, size_t id);

/**
 * Returns the function declaration with the provided id or null if it doesn't exist.
 * @param analyser
 * @param id Id of the function
 * @return FunctionDeclaration* | null
 */
FunctionDeclaration /* | null*/* Analyser_getFunctionById(Analyser *analyser, size_t id);

/**
 * Returns the variable declaration with the provided id or null if it doesn't exist.
 * @param analyser
 * @param id Id of the variable
 * @return VariableDeclaration* | null
 */
VariableDeclaration /* | null*/* Analyser_getVariableById(Analyser *analyser, size_t id);

/**
 * Returns the variable declaration with the provided name,
 * reachable from provided scope or null if it doesn't exist.
 * @param analyser
 * @param name Name of the function
 * @param scope Scope to search from
 * @return FunctionDeclaration* | null
 */
VariableDeclaration /* | null*/* Analyser_getVariableByName(Analyser *analyser, char *name, BlockScope *scope);

/**
 * Returns the function declaration overloads with the provided name or null if it doesn't exist.
 * @param analyser
 * @param name Name of the function
 * @return Array<FunctionDeclaration>* | null
 */
Array /*<FunctionDeclaration> | null*/* Analyser_getFunctionDeclarationsByName(Analyser *analyser, char *name);


/**
 * Reserves and returns a new id.
 * @param analyser
 * @return size_t
 */
size_t Analyser_nextId(Analyser *analyser);

/**
 * Determines whether the provided declaration is global.
 * @param analyser
 * @param declaration Id of the declaration
 * @return bool
 */
bool Analyser_isDeclarationGlobal(Analyser *analyser, size_t id);

/**
 * Returns the built-in function enum from the provided id.
 * @param analyser
 * @param declaration Id of the declaration
 * @return enum BuiltInFunction
 */
enum BuiltInFunction Analyser_getBuiltInFunctionById(Analyser *analyser, size_t id);


/**
 * Returns the type of the provided token.
 * @param tokenKind Kind of the token
 */
ValueType Analyser_getTypeFromToken(enum TokenKind tokenKind);

/**
 * Converts the provided string type to BuiltInType enum
 * @param name Name of the type
 */
enum BuiltInType Analyser_resolveBuiltInType(String *name);


/* Declarations */
/**
 * Allocates a new VariableDeclaration instance.
 * @param analyser
 * @param node
 * @param isConstant
 * @param type
 * @param name
 * @param isUserDefined
 * @param isInitialized
 * @return VariableDeclaration*
 */
VariableDeclaration* new_VariableDeclaration(Analyser *analyser, struct VariableDeclaratorASTNode *node, bool isConstant, ValueType type, String *name, bool isUserDefined, bool isInitialized);

/**
 * Allocates a new FunctionDeclaration instance.
 * @param analyser
 * @param node
 * @return FunctionDeclaration*
 */
FunctionDeclaration* new_FunctionDeclaration(Analyser *analyser, struct FunctionDeclarationASTNode *node);

/**
 * Frees a VariableDeclaration instance.
 * @param declaration
 */
void VariableDeclaration_free(VariableDeclaration *declaration);

/**
 * Frees a FunctionDeclaration instance.
 * @param declaration
 */
void FunctionDeclaration_free(FunctionDeclaration *declaration);


/* BlockScope */
/**
 * Allocates a new BlockScope instance.
 * @param parent
 * @return BlockScope*
 */
BlockScope* BlockScope_alloc(BlockScope *parent);

#endif

/** End of file include/compiler/analyser/Analyser.h **/
