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
} Codegen;


/**
 * @brief Initializes a Codegen instance with the specified Analyser.
 *
 * This function constructs a Codegen instance, associating it with a given Analyser.
 * The Analyser is crucial for code generation as it provides necessary information
 * from the analysis phase. Additionally, the function sets the initial frame of the
 * Codegen instance to FRAME_GLOBAL.
 *
 * @param codegen A pointer to the Codegen instance to be initialized.
 * @param analyser A pointer to the Analyser instance to be associated with the Codegen.
 */
void Codegen_constructor(Codegen *codegen, Analyser *analyser);


/**
 * @brief Dissociates resources for a Codegen instance.
 *
 * @param codegen A pointer to the Codegen instance to be destructed.
 */
void Codegen_destructor(Codegen *codegen);

void Codegen_generate(Codegen *codegen);

#endif // CODEGEN_H

/** End of file include/compiler/codegen/Codegen.h **/
