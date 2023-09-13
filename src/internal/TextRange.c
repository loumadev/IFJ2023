#include <string.h>
#include "utils.h"
#include "internal/String.h"
#include "internal/TextRange.h"


void TextRange_constructor(TextRange *range, char *start, char *end, int line, int column) {
	if(!range) return;

	range->start = start;
	range->end = end;
	range->line = line;
	range->column = column;
	range->length = end - start;
}

void TextRange_destructor(TextRange *range) {
	if(!range) return;

	range->start = NULL;
	range->end = NULL;
	range->length = 0;
}

size_t TextRange_compare(TextRange *range, char *str) {
	if(!range) return 0;
	if(!str) return 0;

	char *ptr = strstr(range->start, str);
	if(!ptr) return 0;

	return ptr - range->start;
}

String* TextRange_getString(TextRange *range) {
	if(!range) return NULL;

	return String_fromSubstring(range->start, 0, range->end - range->start);
}

void TextRange_print(TextRange *range, unsigned int depth, int isProperty) {
	if(!range) {
		if(!isProperty) indent(depth);
		printf("TextRange { NULL }\n");
		return;
	}

	if(!isProperty) indent(depth);
	printf("TextRange {\n");

	indent(depth + 1);
	printf("start: %p\n", range->start);

	indent(depth + 1);
	printf("end: %p\n", range->end);

	indent(depth + 1);
	printf("line: %d\n", range->line);

	indent(depth + 1);
	printf("column: %d\n", range->column);

	indent(depth + 1);
	printf("length: %zu\n", range->length);

	indent(depth + 1);
	printf("__text__: ");
	print_string(range->start, range->end);
	putchar('\n');

	indent(depth);
	printf("}\n");
}