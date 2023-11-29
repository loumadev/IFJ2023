#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "internal/String.h"
#include "compiler/codegen/Codegen.h"

#define INSTRUCTION_NULLARY(ins) \
	fprintf(stdout, "%s\n", ins);

#define NEWLINE \
    fprintf(stdout, "\n");

#define COMMENT(comment) \
    fprintf(stdout, "# %s\n", comment);

#define COMMENT_VAR(id) \
    fprintf(stdout, "# Variable %lu\n", id);

#define COMMENT_WHILE(id) \
    fprintf(stdout, "# While statement %lu\n", id);

#define COMMENT_IF(id) \
    fprintf(stdout, "# If statement %lu\n", id);

#define COMMENT_IF_BLOCK(id) \
    fprintf(stdout, "# If %lu block\n", id);

#define COMMENT_ELSE_BLOCK(id) \
    fprintf(stdout, "# If %lu else\n", id);

#define HEADER \
    fprintf(stdout, ".IFJcode23\n");

// TODO: Fix
void Instruction_pops_where(char * where, enum Frame frame);

void Instruction_defvar_where(char * where, enum Frame frame);

// --- UTILS ---

void Instruction_label_while_start(size_t id);

void Instruction_label_while_end(size_t id);

void Instruction_jumpifneqs_while_end(size_t id);

void Instruction_jump_while_start(size_t id);

void Instruction_jumpifneqs_if_else(size_t id);

void Instruction_label_if_else(size_t id);

void Instruction_jump_if_else(size_t id);

void Instruction_jump_if_end(size_t id);

void Instruction_label_if_end(size_t id);

void Instruction_popretvar(size_t id, enum Frame frame);

void Instruction_pushs_var_named(char * var, enum Frame frame);

// --- INSTUCTIONS ---

void Instruction_return();

void Instruction_pushframe();

void Instruction_readString(char *var, enum Frame frame);

void Instruction_readInt(char *var, enum Frame frame);

void Instruction_readFloat(char *var, enum Frame frame);

void Instruction_write(char * id, enum Frame frame);

void Instruction_defvar(size_t id, enum Frame frame);

void Instruction_defretvar(size_t id, enum Frame frame);

void Instruction_pushs_nil();

void Instruction_pushs_bool(bool value);

void Instruction_pushs_int(long value);

void Instruction_pushs_float(double value);

void Instruction_pushs_string(String * string);

void Instruction_pushs_var(size_t id, enum Frame frame);

void Instruction_pops(size_t id, enum Frame frame);

void Instruction_clears();

void Instruction_adds();

void Instruction_subs();

void Instruction_muls();

void Instruction_divs();

void Instruction_lts();

void Instruction_gts();

void Instruction_eqs();

void Instruction_ands();

void Instruction_ors();

void Instruction_nots();

void Instruction_jump_func_end(size_t id);

void Instruction_label_func_start(size_t id);

void Instruction_int2floats();

void Instruction_float2ints();

void Instruction_strlen(enum Frame resultScope, char *result, char *input, enum Frame inputScope);

void Instruction_int2char(enum Frame resultScope, char *result, enum Frame inputScope, char *input);

void Instruction_stri2int(enum Frame resultScope, char *result, enum Frame inputScope, char *input, int index);

void Instruction_label(char *label);

void Instruction_jump_ifeqs_chr_end();

void Instruction_jump(char *label);

void Instruction_move(enum Frame destinationScope, char* destination, enum Frame sourceScope, char* source);

void Instruction_popframe();

void Instruction_createframe();

void Instruction_call(char *label);

void Instruction_return();

void Instruction_jump_ifeqs(char *label);

void Instruction_getchar(enum Frame resultScope, char *result, enum Frame inputScope, char *input, enum Frame indexScope, char *index);

void Instruction_concat(enum Frame resultScope, char *result, enum Frame input1Scope, char *input1, enum Frame input2Scope, char *input2);

#endif // INSTRUCTION_H
