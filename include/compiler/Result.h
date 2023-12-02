/**
 * @file include/compiler/Result.h
 * @author Author Name <xlogin00@stud.fit.vutbr.cz>
 * @brief This file is part of the IFJ23 project.
 * @copyright Copyright (c) 2023
 */

#include <stdbool.h>
#include "internal/String.h"
#include "internal/Array.h"

#ifndef RESULT_H
#define RESULT_H

enum Severity {
	SEVERITY_NONE = 0,
	SEVERITY_ERROR,
	SEVERITY_WARNING,
	SEVERITY_INFO
};

enum ResultType {
	RESULT_INVALID = -3,
	RESULT_NO_MATCH = -2,
	RESULT_ASSERTION = -1,
	RESULT_SUCCESS = 0,
	RESULT_ERROR_LEXICAL_ANALYSIS = 1,                    // Chybná struktura aktuálního lexému
	RESULT_ERROR_SYNTACTIC_ANALYSIS = 2,                  // Chybná syntaxe programu, chybějící hlavička, atp.
	RESULT_ERROR_SEMANTIC_UNDEFINED_FUNCTION = 3,         // Nedefinovaná funkce
	RESULT_ERROR_SEMANTIC_VARIABLE_REDEFINITION = 3,      // Redefinice proměnné
	RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL_TYPE = 4, // Špatný počet/typ parametrů u volání funkce
	RESULT_ERROR_SEMANTIC_INVALID_RETURN_TYPE = 4,        // Špatný typ návratové hodnoty z funkce
	RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE = 5,         // Použití nedefinované nebo neinicializované proměnné
	RESULT_ERROR_SEMANTIC_INVALID_RETURN = 6,             // Chybějící/přebývající výraz v příkazu návratu z funkce
	RESULT_ERROR_SEMANTIC_INVALID_TYPE = 7,               // Chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
	RESULT_ERROR_SEMANTIC_FAILED_INFER = 8,               // Typ proměnné nebo parametru není uveden a nelze odvodit od použitého výrazu
	RESULT_ERROR_SEMANTIC_OTHER = 9,                      // Ostatní sémantické chyby
	RESULT_ERROR_INTERNAL = 99                            // Interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.)
};

typedef struct Result {
	bool success;
	enum ResultType type;
	enum Severity severity;
	String *message;
	Array /*<Token*>*/ *markers;
	// More properties added by subclasses
} Result;

void Result_constructor(
	Result *result,
	enum ResultType type,
	enum Severity severity,
	String *message,
	Array *markers
);
void Result_destructor(Result *result);

#endif

/** End of file include/compiler/Result.h **/
