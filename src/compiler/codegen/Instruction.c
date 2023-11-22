#include <stdio.h>
#include <stdbool.h>

#include "../../../include/compiler/codegen/Instruction.h"
#include "../../../include/assertf.h"
#include "../../../include/internal/String.h"

void Instruction_move(size_t targetVar, char *targetFrame, size_t sourceVar, char *sourceFrame) {
    fprintf(stdout, "MOVE %s@%lu %s@%lu\n", targetFrame, targetVar, sourceFrame, sourceVar);
}

//
//void Instruction_createframe() {
//	INSTRUCTION_NULLARY("CREATEFRAME")
//}
//
//void Instruction_pushframe() {
//	INSTRUCTION_NULLARY("PUSHFRAME")
//}
//
//void Instruction_popframe() {
//	INSTRUCTION_NULLARY("POPFRAME")
//}
//
void Instruction_defvar(size_t id, char *frame) {
    fprintf(stdout, "DEFVAR %s@%lu\n", frame, id);
}
//
////void Instruction_call(char *label) {
////	INSTRUCTION_UNARY("CALL", label)
////}
//
//void Instruction_return() {
//	INSTRUCTION_NULLARY("RETURN")
//}
//

/// --- PUSH COMMANDS ---
void Instruction_pushs(char *symb) {
    fprintf(stdout, "PUSHS %s\n", symb);
}

void Instruction_pushs_nil() {
    fprintf(stdout, "PUSHS nil@nil\n");
}

void Instruction_pushs_bool(bool value) {
    fprintf(stdout, "PUSHS bool@%s\n", value ? "true" : "false");
}

void Instruction_pushs_int(long value) {
    // TODO: This should be int@%d
    fprintf(stdout, "PUSHS int@%ld\n", value);
}

void Instruction_pushs_float(double value) {
    fprintf(stdout, "PUSHS float@%a\n", value);
}

void Instruction_pushs_string(String *string) {
    fprintf(stdout, "PUSHS string@%s\n", string->value);
}

//
//void Instruction_pops(char *var) {
//	INSTRUCTION_UNARY("POPS", var)
//}
//
//void Instruction_clears() {
//	INSTRUCTION_NULLARY("CLEARS")
//}
//
//void Instruction_add(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("ADD", var, symb1, symb2)
//}
//
//void Instruction_sub(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("SUB", var, symb1, symb2)
//}
//
//void Instruction_mul(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("MUL", var, symb1, symb2)
//}
//
//void Instruction_idiv(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("IDIV", var, symb1, symb2)
//}
//
//void Instruction_adds() {
//	INSTRUCTION_NULLARY("ADDS")
//}
//
//void Instruction_subs() {
//	INSTRUCTION_NULLARY("SUBS")
//}
//
//void Instruction_muls() {
//	INSTRUCTION_NULLARY("MULS")
//}
//
//void Instruction_divs() {
//	INSTRUCTION_NULLARY("DIVS")
//}
//
//void Instruction_idivs() {
//	INSTRUCTION_NULLARY("IDIVS")
//}
//
//void Instruction_lt(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("LT", var, symb1, symb2)
//}
//
//void Instruction_gt(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("GT", var, symb1, symb2)
//}
//
//void Instruction_eq(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("EQ", var, symb1, symb2)
//}
//
//void Instruction_lts() {
//	INSTRUCTION_NULLARY("LTS")
//}
//
//void Instruction_gts() {
//	INSTRUCTION_NULLARY("GTS")
//}
//
//void Instruction_eqs() {
//	INSTRUCTION_NULLARY("EQS")
//}
//
//void Instruction_and(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("AND", var, symb1, symb2)
//}
//
//void Instruction_or(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("OR", var, symb1, symb2)
//}
//
//void Instruction_not(char *var, char *symb) {
//	INSTRUCTION_BINARY("NOT", var, symb)
//}
//
//void Instruction_ands() {
//	INSTRUCTION_NULLARY("ANDS")
//}
//
//void Instruction_ors() {
//	INSTRUCTION_NULLARY("ORS")
//}
//
//void Instruction_nots() {
//	INSTRUCTION_NULLARY("NOTS")
//}
//
//void Instruction_int2float(char *var, char *symb) {
//	INSTRUCTION_BINARY("INT2FLOAT", var, symb)
//}
//
//void Instruction_float2int(char *var, char *symb) {
//	INSTRUCTION_BINARY("FLOAT2INT", var, symb)
//}
//
//void Instruction_int2char(char *var, char *symb) {
//	INSTRUCTION_BINARY("INT2CHAR", var, symb)
//}
//
//void Instruction_stri2int(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("STRI2INT", var, symb1, symb2)
//}
//
//void Instruction_int2floats() {
//	INSTRUCTION_NULLARY("INT2FLOATS")
//}
//
//void Instruction_float2ints() {
//	INSTRUCTION_NULLARY("FLOAT2INTS")
//}
//
//void Instruction_int2chars() {
//	INSTRUCTION_NULLARY("INT2CHARS")
//}
//
//void Instruction_stri2ints() {
//	INSTRUCTION_NULLARY("STRI2INTS")
//}
//
//void Instruction_read(char *var, char *type) {
//	INSTRUCTION_BINARY("READ", var, type)
//}
//
//void Instruction_write(char *symb) {
//	INSTRUCTION_UNARY("WRITE", symb)
//}
//
//void Instruction_concat(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("CONCAT", var, symb1, symb2)
//}
//
//void Instruction_strlen(char *var, char *symb) {
//	INSTRUCTION_BINARY("STRLEN", var, symb)
//}
//
//void Instruction_getchar(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("GETCHAR", var, symb1, symb2)
//}
//
//void Instruction_setchar(char *var, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("SETCHAR", var, symb1, symb2)
//}
//
//void Instruction_type(char *var, char *symb) {
//	INSTRUCTION_BINARY("TYPE", var, symb)
//}
//
//void Instruction_label(char *label) {
//    INSTRUCTION_UNARY("LABEL", label)
//}
//
//void Instruction_jump(char *label) {
//	INSTRUCTION_UNARY("JUMP", label)
//}
//
//void Instruction_jumpifeq(char *label, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("JUMPIFEQ", label, symb1, symb2)
//}
//
//void Instruction_jumpifneq(char *label, char *symb1, char *symb2) {
//	INSTRUCTION_TERNARY("JUMPIFNEQ", label, symb1, symb2)
//}
//
//void Instruction_jumpifeqs(char *label) {
//	INSTRUCTION_UNARY("JUMPIFEQS", label)
//}
//
//void Instruction_jumpifneqs(char *label) {
//	INSTRUCTION_UNARY("JUMPIFNEQS", label)
//}
//
//void Instruction_exit(char *symb) {
//	INSTRUCTION_UNARY("EXIT", symb)
//}
//
//void Instruction_break() {
//	INSTRUCTION_NULLARY("BREAK")
//}
//
//void Instruction_dprint(char *symb) {
//	INSTRUCTION_UNARY("DPRINT", symb)
//}
