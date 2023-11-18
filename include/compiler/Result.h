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
	RESULT_ERROR_LEXICAL_ANALYSIS = 1,                  // Chybná struktura aktuálního lexému
	RESULT_ERROR_SYNTACTIC_ANALYSIS = 2,                // Chybná syntaxe programu, chybějící hlavička, atp.
	RESULT_ERROR_SEMANTIC_FUNCTION_DEFINITION = 3,      // Nedefinovaná funkce, pokus o redefinice funkce
	RESULT_ERROR_SEMANTIC_INVALID_FUNCTION_CALL = 4,    // Špatný počet/typ parametrů u volání funkce či typ návratové hodnoty z funkce
	RESULT_ERROR_SEMANTIC_UNDEFINED_VARIABLE = 5,       // Použití nedefinované proměnné
	RESULT_ERROR_SEMANTIC_INVALID_RETURN = 6,           // Chybějící/přebývající výraz v příkazu návratu z funkce
	RESULT_ERROR_SEMANTIC_INVALID_TYPE = 7,             // Chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
	RESULT_ERROR_SEMANTIC_OTHER = 8,                    // Ostatní sémantické chyby
	RESULT_ERROR_INTERNAL = 99                          // Interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.)
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
