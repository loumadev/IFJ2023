/**
 * @file src/compiler/codegen/Instruction.c
 * @author Jaroslav Novotny <xnovot2r@stud.fit.vutbr.cz>
 * @brief Implementation of instruction set.
 * @copyright Copyright (c) 2023
 */

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
		unsigned char c = string->value[i];
		if(c < 10) {
			String *replacement = String_fromFormat("\\00%d", c);
			String_append(&buffer, replacement->value);
			String_free(replacement);
		} else if(c <= 32 || c == 35 || c == 92) {
			String *replacement = String_fromFormat("\\0%d", c);
			String_append(&buffer, replacement->value);
			String_free(replacement);
		} else if (c > 126) {
            String *replacement = String_fromFormat("\\%d", c);
            String_append(&buffer, replacement->value);
            String_free(replacement);
        } else {
			String_appendChar(&buffer, c);
		}
	}

	String_set(string, buffer.value);
}

// --- INSTUCTIONS ---

void Instruction_pushframe() {
	INSTRUCTION_NULLARY("PUSHFRAME")
}

void Instruction_defvar_id(size_t id, enum Frame frame) {
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
void Instruction_pops(char *where, enum Frame frame) {
	fprintf(stdout, "POPS %s@$%s\n", __Instruction_getFrame(frame), where);
}

void Instruction_defvar(char *where, enum Frame frame) {
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

void Instruction_pushs_id(size_t id, enum Frame frame) {
	fprintf(stdout, "PUSHS %s@$%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_pushs(char *var, enum Frame frame) {
	fprintf(stdout, "PUSHS %s@$%s\n", __Instruction_getFrame(frame), var);
}


void Instruction_pops_id(size_t id, enum Frame frame) {
	fprintf(stdout, "POPS %s@$%lu\n", __Instruction_getFrame(frame), id);
}

void Instruction_pops_named_id(char * name, size_t id, enum Frame frame) {
    fprintf(stdout, "POPS %s@$%s_%lu\n", __Instruction_getFrame(frame), name, id);
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

void Instruction_jump_ifeqs_chr_end() {
	fprintf(stdout, "JUMPIFEQS $chr_empty\n");
}

void Instruction_jump(char *label) {
	fprintf(stdout, "JUMP $%s\n", label);
}

void Instruction_move_vars(enum Frame destinationScope, char *destination, enum Frame sourceScope, char *source) {
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

void Instruction_getchar(enum Frame resultScope, char *result, enum Frame inputScope, char *input, enum Frame indexScope, char *index) {
	fprintf(stdout, "GETCHAR %s@$%s %s@$%s %s@$%s\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(inputScope), input, __Instruction_getFrame(indexScope), index);
}

void Instruction_concat(enum Frame resultScope, char *result, enum Frame input1Scope, char *input1, enum Frame input2Scope, char *input2) {
	fprintf(stdout, "CONCAT %s@$%s %s@$%s %s@$%s\n", __Instruction_getFrame(resultScope), result, __Instruction_getFrame(input1Scope), input1, __Instruction_getFrame(input2Scope), input2);
}

void Instruction_call_func(size_t id) {
	fprintf(stdout, "CALL $func_%lu\n", id);
}

void Instruction_label_func(size_t id) {
	fprintf(stdout, "LABEL $func_%lu\n", id);
}

void Instruction_pushs_func_result(size_t id) {
	fprintf(stdout, "PUSHS TF@$ret_%lu\n", id);
}

void Instruction_move_id(enum Frame destinationScope, size_t destination, enum Frame sourceScope, size_t source) {
	fprintf(stdout, "MOVE %s@$%lu %s@$%lu\n", __Instruction_getFrame(destinationScope), destination, __Instruction_getFrame(sourceScope), source);
}

void Instruction_move_int(enum Frame destinationScope, char *destination, int value) {
	fprintf(stdout, "MOVE %s@$%s int@%d\n", __Instruction_getFrame(destinationScope), destination, value);
}

void Instruction_move_string(enum Frame destinationScope, char *destination, String *value) {
	__Instruction_escape_string(value);
	fprintf(stdout, "MOVE %s@$%s string@%s\n", __Instruction_getFrame(destinationScope), destination, value->value);
}

void Instruction_move_nil(enum Frame destinationScope, char *destination) {
	fprintf(stdout, "MOVE %s@$%s nil@nil\n", __Instruction_getFrame(destinationScope), destination);
}

void Instruction_move_int_id(enum Frame destinationScope, size_t destination, long int value) {
	fprintf(stdout, "MOVE %s@$%lu int@%ld\n", __Instruction_getFrame(destinationScope), destination, value);
}

void Instruction_move_string_id(enum Frame destinationScope, size_t destination, String *value) {
	__Instruction_escape_string(value);
	fprintf(stdout, "MOVE %s@$%lu string@%s\n", __Instruction_getFrame(destinationScope), destination, value->value);
}

void Instruction_move_nil_id(enum Frame destinationScope, size_t destination) {
	fprintf(stdout, "MOVE %s@$%lu nil@nil\n", __Instruction_getFrame(destinationScope), destination);
}

void Instruction_move_float_id(enum Frame destinationScope, size_t destination, double value) {
	fprintf(stdout, "MOVE %s@$%lu float@%a\n", __Instruction_getFrame(destinationScope), destination, value);
}

void Instruction_move_bool_id(enum Frame destinationScope, size_t destination, bool value) {
	fprintf(stdout, "MOVE %s@$%lu bool@%s\n", __Instruction_getFrame(destinationScope), destination, value ? "true" : "false");
}

void Instruction_add_int(enum Frame destinationScope, char *destination, enum Frame sourceScope, char *source, int value) {
	fprintf(stdout, "ADD %s@$%s %s@$%s int@%d\n", __Instruction_getFrame(destinationScope), destination, __Instruction_getFrame(sourceScope), source, value);
}

void Instruction_add_int_id(enum Frame destinationScope, size_t destination, enum Frame sourceScope, size_t source, int value) {
    fprintf(stdout, "ADD %s@$%lu %s@$%lu int@%d\n", __Instruction_getFrame(destinationScope), destination, __Instruction_getFrame(sourceScope), source, value);
}

void Instruction_label_id(char *label, size_t id) {
    fprintf(stdout, "LABEL $%s_%lu\n", label, id);
}

void Instruction_jump_id(char *label, size_t id) {
    fprintf(stdout, "JUMP $%s_%lu\n", label, id);
}

void Instruction_jump_ifeqs_id(char *label, size_t id) {
    fprintf(stdout, "JUMPIFEQS $%s_%lu\n", label, id);
}

void Instruction_jump_ifneqs_id(char *label, size_t id) {
    fprintf(stdout, "JUMPIFNEQS $%s_%lu\n", label, id);
}

/** End of file src/compiler/codegen/Instruction.c **/
