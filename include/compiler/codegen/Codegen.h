#include <stdio.h>

#include "compiler/parser/ASTNodes.h"

#ifndef CODEGEN_H
#define CODEGEN_H

#define TAB "\009"

typedef struct Codegen {
    Analyser *analyser;
} Codegen;


enum Frame {
    FRAME_GLOBAL,
    FRAME_LOCAL,
    FRAME_TEMPORARY,
};

void Codegen_constructor(Codegen *codegen, Analyser *analyser);

void Codegen_destructor(Codegen *codegen);

void Codegen_generate(Codegen *codegen);

void Codegen_escapeString(String *string);

#endif // CODEGEN_H
