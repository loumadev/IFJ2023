#include <stdio.h>

#include "compiler/parser/ASTNodes.h"

#ifndef CODEGEN_H
#define CODEGEN_H

#define TAB "\009"
// #define NEWLINE "\010"
#define SPACE "\032"
#define EXCLAMATION "\033"
#define QUOTATION "\034"
#define HASH "\035"
#define DOLLAR "\036"
#define PERCENT "\037"
#define AMPERSAND "\038"
#define APOSTROPHE "\039"
#define LEFT_PARENTHESIS "\040"
#define RIGHT_PARENTHESIS "\041"
#define ASTERISK "\042"
#define PLUS "\043"
#define COMMA "\044"
#define MINUS "\045"
#define PERIOD "\046"
#define SLASH "\047"
#define COLON "\072"
#define SEMICOLON "\073"
#define LESS_THAN "\074"
#define EQUALS "\075"
#define GREATER_THAN "\076"
#define QUESTION "\077"
#define AT "\100"
#define LEFT_BRACKET "\133"
#define BACKSLASH "\134"
#define RIGHT_BRACKET "\135"
#define CARET "\136"
#define UNDERSCORE "\137"
#define GRAVE_ACCENT "\140"
#define LEFT_BRACE "\173"
#define VERTICAL_BAR "\174"
#define RIGHT_BRACE "\175"
#define TILDE "\176"

typedef struct Codegen {
    //SymbolTable *symbolTable;
    ProgramASTNode *program;
    FILE *output;
} Codegen;


void Codegen_constructor(Codegen *codegen);
void Codegen_destructor(Codegen *codegen);
void Codegen_setProgram(Codegen *codegen, ProgramASTNode *program);
void Codegen_setOutput(Codegen *codegen, FILE *output);
void Codegen_generate(Codegen *codegen);
char * Codegen_escapeString(char * string);

#endif // CODEGEN_H
