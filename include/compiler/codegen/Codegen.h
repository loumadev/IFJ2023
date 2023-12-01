#include <stdio.h>

#include "compiler/parser/ASTNodes.h"

#ifndef CODEGEN_H
#define CODEGEN_H

enum Frame {
	FRAME_GLOBAL,
	FRAME_LOCAL,
	FRAME_TEMPORARY
};

typedef struct Codegen {
	Analyser *analyser;
	enum Frame frame;
    enum BuiltInTypes lastPushedType;
    bool containsReturns;
} Codegen;

void Codegen_constructor(Codegen *codegen, Analyser *analyser);

void Codegen_destructor(Codegen *codegen);

void Codegen_generate(Codegen *codegen);

#endif // CODEGEN_H
