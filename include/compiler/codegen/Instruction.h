#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "internal/String.h"

#define INSTRUCTION_NULLARY(ins) \
	fprintf(stdout, "%s\n", ins);

#define INSTRUCTION_UNARY(ins, arg1) \
	fprintf(stdout, "%s %lu\n", ins, arg1);

#define INSTRUCTION_BINARY(ins, arg1, arg2) \
	fprintf(stdout, "%s %s %s\n", ins, arg1, arg2);

#define INSTRUCTION_TERNARY(ins, arg1, arg2, arg3) \
	fprintf(stdout, "%s %s %s %s\n", ins, arg1, arg2, arg3);

#define NEWLINE \
    fprintf(stdout, "\n");

#define COMMENT(comment) \
    fprintf(stdout, "# %s\n", comment);

#define COMMENT_VAR(id) \
    fprintf(stdout, "# Variable %lu\n", id);

#define HEADER \
    fprintf(stdout, ".IFJcode23\n");

void Instruction_move(size_t targetVar, char* targetFrame, size_t sourceVar, char* sourceFrame);

void Instruction_move_value(size_t targetVar, char* targetFrame, char* value);

void Instruction_createframe();

void Instruction_pushframe();

void Instruction_popframe();

void Instruction_defvar(size_t id, char *frame);

void Instruction_call(char *label);

void Instruction_return();

void Instruction_pushs(char *symb);

void Instruction_pushs_nil();

void Instruction_pushs_bool(bool value);

void Instruction_pushs_int(long value);

void Instruction_pushs_float(double value);

void Instruction_pushs_string(String * string);

void Instruction_pops(char *var);

void Instruction_clears();

void Instruction_add(char *var, char *symb1, char *symb2);

void Instruction_sub(char *var, char *symb1, char *symb2);

void Instruction_mul(char *var, char *symb1, char *symb2);

void Instruction_idiv(char *var, char *symb1, char *symb2);

void Instruction_adds();

void Instruction_subs();

void Instruction_muls();

void Instruction_divs();

void Instruction_idivs();

void Instruction_lt(char *var, char *symb1, char *symb2);

void Instruction_gt(char *var, char *symb1, char *symb2);

void Instruction_eq(char *var, char *symb1, char *symb2);

void Instruction_lts();

void Instruction_gts();

void Instruction_eqs();

void Instruction_and(char *var, char *symb1, char *symb2);

void Instruction_or(char *var, char *symb1, char *symb2);

void Instruction_not(char *var, char *symb);

void Instruction_ands();

void Instruction_ors();

void Instruction_nots();

void Instruction_int2float(char *var, char *symb);

void Instruction_float2int(char *var, char *symb);

void Instruction_int2floats();

void Instruction_float2ints();

void Instruction_int2char(char *var, char *symb);

void Instruction_stri2int(char *var, char *symb1, char *symb2);

void Instruction_int2chars();

void Instruction_stri2ints();

void Instruction_read(char *var, char *type);

void Instruction_write(char *symb);

void Instruction_concat(char *var, char *symb1, char *symb2);

void Instruction_strlen(char *var, char *symb);

void Instruction_getchar(char *var, char *symb1, char *symb2);

void Instruction_setchar(char *var, char *symb1, char *symb2);

void Instruction_type(char *var, char *symb);

void Instruction_label(char *label);

void Instruction_jump(char *label);

void Instruction_jumpifeq(char *label, char *symb1, char *symb2);

void Instruction_jumpifneq(char *label, char *symb1, char *symb2);

void Instruction_jumpifeqs(char *label);

void Instruction_jumpifneqs(char *label);

void Instruction_exit(char *symb);

void Instruction_break();

void Instruction_dprint(char *symb);

#endif // INSTRUCTION_H
