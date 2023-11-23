#include <stdio.h>
#include <stdbool.h>

#include "../../../include/compiler/codegen/Instruction.h"
#include "../../../include/assertf.h"
#include "../../../include/internal/String.h"
#include "../../../include/compiler/codegen/Codegen.h"

char *__Instruction_getFrame(enum Frame frame);

char *__Instruction_getFrame(enum Frame frame) {
    switch (frame) {
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

void Instruction_jump_while_start(size_t id){
    fprintf(stdout, "JUMP $while_start_%lu\n", id);
}

void Instruction_jumpifneqs_if_end(size_t id) {
    fprintf(stdout, "JUMPIFNEQS $if_end_%lu\n", id);
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

void Instruction_pushs_var(size_t id, enum Frame frame) {
    fprintf(stdout, "PUSHS %s@$%lu\n", __Instruction_getFrame(frame), id);
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

void Instruction_label(char *label) {
    fprintf(stdout, "LABEL %s\n", label);
}

void Instruction_jump(char *label) {
    fprintf(stdout, "JUMP %s\n", label);
}

void Instruction_jump_func_end(size_t id) {
    fprintf(stdout, "JUMP $%lu_end\n", id);
}


void Instruction_label_func_start(size_t id) {
    fprintf(stdout, "LABEL $%lu_start\n", id);
}

void Instruction_label_func_end(size_t id) {
    fprintf(stdout, "LABEL $%lu_end\n", id);
}
