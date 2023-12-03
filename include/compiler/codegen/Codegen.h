/**
 * @file include/compiler/codegen/Codegen.h
 * @author Jaroslav Novotny <xnovot2r@stud.fit.vutbr.cz>
 * @brief Header containing function definitions for code generator.
 * @copyright Copyright (c) 2023
 */

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
    enum BuiltInType lastPushedType;
    bool containsReturns;
} Codegen;

void Codegen_constructor(Codegen *codegen, Analyser *analyser);

void Codegen_destructor(Codegen *codegen);

void Codegen_generate(Codegen *codegen);

#endif // CODEGEN_H

/** End of file include/compiler/codegen/Codegen.h **/
