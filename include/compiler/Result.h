#include <stdbool.h>
#include "internal/String.h"
#include "internal/Array.h"

#ifndef PARSER_H
#define PARSER_H

enum Severity {
	SEVERITY_NONE = 0,
	SEVERITY_ERROR,
	SEVERITY_WARNING,
	SEVERITY_INFO
};


/*

   • 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
   • 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.).
   • 3 - sémantická chyba v programu – nedefinovaná funkce, pokus o redefinice funkce.
   • 4 - sémantická/běhová chyba v programu – špatný počet/typ parametrů u volání funkce či typ návratové hodnoty z funkce.
   • 5 - sémantická chyba v programu – použití nedefinované proměnné.
   • 6 - sémantická/běhová chyba v programu – chybějící/přebývající výraz v příkazu návratu z funkce.
   • 7 - sémantická/běhová chyba typové kompatibility v aritmetických, řetězcových a
   relačních výrazech.
   • 8 - ostatní sémantické chyby.
   • 99 - interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.).



 */
enum ResultType {
	RESULT_INVALID = -3,
	RESULT_NO_MATCH = -2,
	RESULT_ASSERTION = -1,
	RESULT_SUCCESS = 0,
	RESULT_ERROR_STATIC_LEXICAL_ANALYSIS = 1,
	RESULT_ERROR_STATIC_SYNTACTIC_ANALYSIS = 2,
	// TODO: Add more error types
	RESULT_ERROR_SEMANTIC_FUNCTION = 3,
	RESULT_ERROR_RUNTIME_UNDEFINED_VARIABLE = 5,
	RESULT_ERROR_SEMANTIC_OTHER = 8,
	RESULT_ERROR_INTERNAL = 99
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
