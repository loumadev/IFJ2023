/**
 * @file src/main.c
 * @author Jaroslav Louma <xlouma00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"


#include "allocator/MemoryAllocator.h"
#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/analyser/Analyser.h"
#include "compiler/codegen/Codegen.h"

#include "colors.h"

#define BUFFER_SIZE 1024

int main(int argc, const char *argv[]) {
	(void)argc;
	(void)argv;

	// Prepare the source string and input buffer
	String *source = String_alloc("");
	char buffer[BUFFER_SIZE];

	// Drain input stream into a buffer and append it to the source
	while(fgets(buffer, BUFFER_SIZE, stdin)) {
		String_append(source, buffer);
	}

	// Prepare the lexer
	Lexer lexer;
	Lexer_constructor(&lexer);
	Lexer_setSource(&lexer, source->value);

	// Prepare the parser
	Parser parser;
	Parser_constructor(&parser, &lexer);

	// Prepare the analyzer
	Analyser analyser;
	Analyser_constructor(&analyser);

	// Prepare the code generator
	Codegen codegen;
	Codegen_constructor(&codegen, &analyser);

	// Parse the source
	ParserResult result = Parser_parse(&parser);
	if(!result.success) {
		// TODO: Add error utils here
		fprintf(stderr, RED BOLD "error: " RST WHITE "%s\n" RST, result.message->value);

		Allocator_cleanup();
		return result.type;
	}

	// Analyse the AST
	AnalyserResult analyserResult = Analyser_analyse(&analyser, (ProgramASTNode*)result.node);
	if(!analyserResult.success) {
		// TODO: Add error utils here
		fprintf(stderr, RED BOLD "error: " RST WHITE "%s\n" RST, analyserResult.message->value);

		Allocator_cleanup();
		return analyserResult.type;
	}

	// Generate the assembly
	Codegen_generate(&codegen);

	Allocator_cleanup();
	return 0;
}

/** End of file src/main.c **/
