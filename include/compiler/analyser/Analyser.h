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

ValueType Analyser_getTypeFromToken(enum TokenKind tokenKind);

void Analyser_constructor(Analyser *analyser);
void Analyser_destructor(Analyser *analyser);
AnalyserResult Analyser_analyse(Analyser *analyser, ProgramASTNode *ast);
bool Analyser_isDeclarationGlobal(Analyser *analyser, size_t id);
Declaration /*<VariableDeclaration | FunctionDeclaration> | null*/* Analyser_getDeclarationById(Analyser *analyser, size_t id);
FunctionDeclaration /* | null*/* Analyser_getFunctionById(Analyser *analyser, size_t id);
VariableDeclaration /* | null*/* Analyser_getVariableById(Analyser *analyser, size_t id);
VariableDeclaration /* | null*/* Analyser_getVariableByName(Analyser *analyser, char *name, BlockScope *scope);
Array /*<FunctionDeclaration> | null*/* Analyser_getFunctionDeclarationsByName(Analyser *analyser, char *name);
enum BuiltInFunction Analyser_getBuiltInFunctionById(Analyser *analyser, size_t id);

enum BuiltInTypes Analyser_resolveBuiltInType(String *name);
AnalyserResult Analyser_resolveExpressionType(Analyser *analyser, ExpressionASTNode *node, BlockScope *scope, ValueType prefferedType, ValueType *outType);

size_t Analyser_nextId(Analyser *analyser);
VariableDeclaration* new_VariableDeclaration(Analyser *analyser, struct VariableDeclaratorASTNode *node, bool isConstant, ValueType type, String *name, bool isUserDefined, bool isInitialized);
FunctionDeclaration* new_FunctionDeclaration(Analyser *analyser, struct FunctionDeclarationASTNode *node);

void VariableDeclaration_free(VariableDeclaration *declaration);
void FunctionDeclaration_free(FunctionDeclaration *declaration);

BlockScope* BlockScope_alloc(BlockScope *parent);

/*
   func f(exter inter: Int) -> Int {
    var b = 20
    var exter = 8;
    var inter = 100;
    return inter + b + exter;
   }
   print(f(exter: 15)) // 128
 */

/*
   func f(exter inter: Int) -> Int {
    inter + 5
   }
   print(f(exter: 15)) // 20
 */

#endif
