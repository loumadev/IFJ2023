#include <stdio.h>

#include "compiler/parser/ASTNodes.h"
#include "compiler/codegen/Instruction.h"
#include "compiler/codegen/Codegen.h"
#include "internal/String.h"
#include "internal/Array.h"

void __Codegen_generatePreamble(Codegen *codegen);

void Codegen_constructor(Codegen *codegen) {
	if(!codegen) return;

    // TODO: Symbol table needed as well
	// SymbolTable_constructor(codegen->symbolTable);
	codegen->program = NULL;
	codegen->output = stdout;
}

void Codegen_destructor(Codegen *codegen) {
	if(!codegen) return;

	// SymbolTable_destructor(codegen->symbolTable);
	codegen->program = NULL;
	codegen->output = NULL;
}

void Codegen_setProgram(Codegen *codegen, ProgramASTNode *program) {
	if(!codegen) return;

	codegen->program = program;
}

// TODO: This is currently doing nothing. It should be used to set the output file.
void Codegen_setOutput(Codegen *codegen, FILE *output) {
	if(!codegen) return;

	codegen->output = output;
}

void Codegen_generate(Codegen *codegen) {
	if(!codegen) return;

	__Codegen_generatePreamble(codegen);

    // TODO: Generate code
}

void __Codegen_generatePreamble(Codegen *codegen) {
	if(!codegen) return;

	COMMENT("Generated by IFJ2023 compiler")
	HEADER
    NEWLINE
}