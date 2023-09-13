#include <stdbool.h>
#include "compiler/lexer/Token.h"
#include "overload.h"

enum Severity {
	SEVERITY_ERROR,
	SEVERITY_WARNING,
	SEVERITY_NOTE
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
	char *message;
	Token *token;
	enum ResultType type;
	enum Severity severity;
} Result;


void Result_constructor(Result *result, enum ResultType type, char *message, Token *token, enum Severity severity);
void Result_destructor(Result *result);

Result __Result_Create(enum ResultType type, char *message, Token *token, enum Severity severity);

#define Result(...) overload(__Result, __VA_ARGS__)
#define __Result1(type) __Result_Create(type, 0, 0, SEVERITY_NOTE)
#define __Result2(type, message) __Result_Create(type, message, 0, SEVERITY_ERROR)
#define __Result3(type, message, token) __Result_Create(type, message, token, SEVERITY_ERROR)
#define __Result4(type, message, token, severity) __Result_Create(type, message, token, severity)
