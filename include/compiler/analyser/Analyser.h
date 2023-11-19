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
	HashMap /*<String, VariableDeclaration>*/ *variables;
	bool isUsed;
	ValueType returnType;
} FunctionDeclaration;

typedef struct BlockScope {
	struct BlockScope *parent;
	HashMap /*<String, VariableDeclaration>*/ *variables;
} BlockScope;

typedef struct Analyser {
	ProgramASTNode *ast;
	BlockScope *globalScope;
	HashMap /*<String, Array<FunctionDeclaration>>*/ *functions; // Relative to global scope
	HashMap /*<String, VariableDeclaration>*/ *variables; // Relative to global scope //! Unused
	HashMap /*<String, String>*/ *types; // TODO: delete this
	HashMap /*<String, Declaration>*/ *idsPool;
	size_t idCounter;
} Analyser;

ValueType Analyser_getTypeFromToken(enum TokenKind tokenKind);

void Analyser_constructor(Analyser *analyser);
void Analyser_destructor(Analyser *analyser);
AnalyserResult Analyser_analyse(Analyser *analyser, ProgramASTNode *ast);
Declaration* Analyser_getDeclarationById(Analyser *analyser, size_t id);
FunctionDeclaration* Analyser_getFunctionById(Analyser *analyser, size_t id);
VariableDeclaration* Analyser_getVariableById(Analyser *analyser, size_t id);
VariableDeclaration* Analyser_getVariableByName(Analyser *analyser, String *name, BlockScope *scope);

enum BuiltInTypes Analyser_resolveBuiltInType(String *name);
AnalyserResult Analyser_resolveExpressionType(Analyser *analyser, ExpressionASTNode *node, BlockScope *scope, ValueType *outType);

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
