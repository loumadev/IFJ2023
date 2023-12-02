#include <stdio.h>
#include <stdbool.h>

#include "assertf.h"
#include "internal/String.h"
#include "compiler/codegen/Instruction.h"
#include "compiler/codegen/Codegen.h"

char * __Instruction_getFrame(enum Frame frame);
void __Instruction_escape_string(String *string);

char * __Instruction_getFrame(enum Frame frame) {
	switch(frame) {
		case FRAME_GLOBAL:
			return "GF";
		case FRAME_LOCAL:
			return "LF";
		case FRAME_TEMPORARY:
			return "TF";
		default:
			fassertf("Invalid frame. Something went wrong.");
	}
}

void __Instruction_escape_string(String *string) {
	String buffer;
	String_constructor(&buffer, "");

	for(size_t i = 0; i < string->length; i++) {
		char c = string->value[i];
		if(c <= 32 || c == 35 || c == 92) {
			String *replacement = String_fromFormat("\\0%d", c);
			String_append(&buffer, replacement->value);
			String_free(replacement);
		} else {
			String_appendChar(&buffer, c);
		}
	}

	String_set(string, buffer.value);
}

/// --- UTILS ---
void Instruction_label_while_start(size_t id) {
	fprintf(stdout, "LABEL $while_start_%lu\n", id);
}

void Instruction_label_while_end(size_t id) {
	fprintf(stdout, "LABEL $while_end_%lu\n", id);
}

void Instruction_jumpifneqs_while_end(size_t id) {
	fprintf(stdout, "JUMPIFNEQS $while_end_%lu\n", id);
}

void Instruction_jump_while_start(size_t id) {
	fprintf(stdout, "JUMP $while_start_%lu\n", id);
}

void Instruction_jumpifneqs_if_else(size_t id) {
	fprintf(stdout, "JUMPIFNEQS $if_else_%lu\n", id);
}

void Instruction_jump_if_end(size_t id) {
	fprintf(stdout, "JUMP $if_end_%lu\n", id);
}

void Instruction_label_if_else(size_t id) {
	fprintf(stdout, "LABEL $if_else_%lu\n", id);
}

void Instruction_jump_if_else(size_t id) {
	fprintf(stdout, "JUMP $if_else_%lu\n", id);
}

void Instruction_label_if_end(size_t id) {
	fprintf(stdout, "LABEL $if_end_%lu\n", id);
}

// --- INSTUCTIONS ---

void Instruction_pushframe() {
	INSTRUCTION_NULLARY("PUSHFRAME")
}

void Instruction_defvar(size_t id, enum Frame frame) {
	fprintf(stdout, "DEFVAR %s@$%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_defretvar(size_t id, enum Frame frame) {
	fprintf(stdout, "DEFVAR %s@$ret_%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_popretvar(size_t id, enum Frame frame) {
	fprintf(stdout, "POPS %s@$ret_%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_return() {
	INSTRUCTION_NULLARY("RETURN")
}

void Instruction_readString(char *var, enum Frame frame) {
	fprintf(stdout, "READ %s@$%s string\n", __Instruction_getFrame(frame), var);
}

void Instruction_readInt(char *var, enum Frame frame) {
	fprintf(stdout, "READ %s@$%s int\n", __Instruction_getFrame(frame), var);
}

void Instruction_readFloat(char *var, enum Frame frame) {
	fprintf(stdout, "READ %s@$%s float\n", __Instruction_getFrame(frame), var);
}

/// --- PUSH COMMANDS ---

void Instruction_pushs_nil() {
	fprintf(stdout, "PUSHS nil@nil\n");
}

void Instruction_pushs_bool(bool value) {
	fprintf(stdout, "PUSHS bool@%s\n", value ? "true" : "false");
}

void Instruction_write(char *id, enum Frame frame) {
	fprintf(stdout, "WRITE %s@$%s\n", __Instruction_getFrame(frame), id);
}

// TODO: This is very bad shortcut, should be fixed
void Instruction_pops_where(char *where, enum Frame frame) {
	fprintf(stdout, "POPS %s@$%s\n", __Instruction_getFrame(frame), where);
}

void Instruction_defvar_where(char *where, enum Frame frame) {
	fprintf(stdout, "DEFVAR %s@$%s\n", __Instruction_getFrame(frame), where);
}

void Instruction_pushs_int(long value) {
	// TODO: This should be int@%d
	fprintf(stdout, "PUSHS int@%ld\n", value);
}

void Instruction_pushs_float(double value) {
	fprintf(stdout, "PUSHS float@%a\n", value);
}

void Instruction_pushs_string(String *string) {
	__Instruction_escape_string(string);
	fprintf(stdout, "PUSHS string@%s\n", string->value);
}

void Instruction_pushs_var(size_t id, enum Frame frame) {
	fprintf(stdout, "PUSHS %s@$%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_pushs_var_named(char *var, enum Frame frame) {
	fprintf(stdout, "PUSHS %s@$%s\n", __Instruction_getFrame(frame), var);
}


void Instruction_pops(size_t id, enum Frame frame) {
	fprintf(stdout, "POPS %s@$%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_clears() {
	INSTRUCTION_NULLARY("CLEARS")
}

void Instruction_adds() {
	INSTRUCTION_NULLARY("ADDS")
}

void Instruction_subs() {
	INSTRUCTION_NULLARY("SUBS")
}

void Instruction_muls() {
	INSTRUCTION_NULLARY("MULS")
}

void Instruction_divs() {
	INSTRUCTION_NULLARY("DIVS")
}

void Instruction_idivs() {
    INSTRUCTION_NULLARY("IDIVS")
}

void Instruction_lts() {
	INSTRUCTION_NULLARY("LTS")
}

void Instruction_gts() {
	INSTRUCTION_NULLARY("GTS")
}

void Instruction_eqs() {
	INSTRUCTION_NULLARY("EQS")
}

void Instruction_ands() {
	INSTRUCTION_NULLARY("ANDS")
}

void Instruction_ors() {
	INSTRUCTION_NULLARY("ORS")
}

void Instruction_nots() {
	INSTRUCTION_NULLARY("NOTS")
}

void Instruction_jump_func_end(size_t id) {
	fprintf(stdout, "JUMP $%lu_end\n", id);
}


void Instruction_label_func_start(size_t id) {
	fprintf(stdout, "LABEL $%lu_start\n", id);
}

void Instruction_int2floats() {
	INSTRUCTION_NULLARY("INT2FLOATS")
}

void Instruction_float2ints() {
	INSTRUCTION_NULLARY("FLOAT2INTS")
}

void Instruction_strlen(enum Frame resultScope, char *result, char *input, enum Frame inputScope) {
	fprintf(stdout, "STRLEN %s@$%s %s@$%s\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(inputScope), input);
}

void Instruction_int2char(enum Frame resultScope, char *result, enum Frame inputScope, char *input) {
    fprintf(stdout, "INT2CHAR %s@$%s %s@$%s\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(inputScope), input);
}

void Instruction_stri2int(enum Frame resultScope, char *result, enum Frame inputScope, char *input, int index) {
    fprintf(stdout, "STRI2INT %s@$%s %s@$%s int@%d\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(inputScope), input, index);
}

void Instruction_label(char *label) {
    fprintf(stdout, "LABEL $%s\n", label);
}

void Instruction_jump_ifeqs_chr_end(){
    fprintf(stdout, "JUMPIFEQS $chr_empty\n");
}

void Instruction_jump(char *label) {
    fprintf(stdout, "JUMP $%s\n", label);
}

void Instruction_move(enum Frame destinationScope, char* destination, enum Frame sourceScope, char* source) {
    fprintf(stdout, "MOVE %s@$%s %s@$%s\n", __Instruction_getFrame(destinationScope), destination, __Instruction_getFrame(sourceScope), source);
}

void Instruction_popframe() {
    INSTRUCTION_NULLARY("POPFRAME")
}

void Instruction_call(char *label) {
    fprintf(stdout, "CALL $%s\n", label);
}

void Instruction_createframe() {
    INSTRUCTION_NULLARY("CREATEFRAME")
}

void Instruction_jump_ifeqs(char *label) {
    fprintf(stdout, "JUMPIFEQS $%s\n", label);
}

void Instruction_getchar(enum Frame resultScope, char *result, enum Frame inputScope, char *input, enum Frame indexScope, char *index){
    fprintf(stdout, "GETCHAR %s@$%s %s@$%s %s@$%s\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(inputScope), input, __Instruction_getFrame(indexScope), index);
}

void Instruction_concat(enum Frame resultScope, char *result, enum Frame input1Scope, char *input1, enum Frame input2Scope, char *input2){
    fprintf(stdout, "CONCAT %s@$%s %s@$%s %s@$%s\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(input1Scope), input1, __Instruction_getFrame(input2Scope), input2);
}

void Instruction_call_func(size_t id){
    fprintf(stdout, "CALL $func_%lu\n", id);
}

void Instruction_label_func(size_t id){
    fprintf(stdout, "LABEL $func_%lu\n", id);
}

void Instruction_move_arg(size_t id) {
    fprintf(stdout, "MOVE LF@$%lu TF@%%0\n", id);
}

void Instruction_pushs_func_result(size_t id){
    fprintf(stdout, "PUSHS TF@$ret_%lu\n", id);
}

void Instruction_move_id(enum Frame destinationScope, size_t destination, enum Frame sourceScope, size_t source) {
    fprintf(stdout, "MOVE %s@$%lu %s@$%lu\n", __Instruction_getFrame(destinationScope), destination, __Instruction_getFrame(sourceScope), source);
}