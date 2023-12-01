#include <string.h>
#include "inspector.h"
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

char TextRange_peek(TextRange *range, int offset) {
	if(!range) return 0;
	if(range->start + offset >= range->end) return 0;

	return range->start[offset];
}

size_t TextRange_compare(TextRange *range, char *str) {
	if(!range) return 0;
	if(!str) return 0;

	size_t len = strlen(str);

	// If the range is not the same length as the string, they cannot be equal
	if((size_t)(range->end - range->start) != len) return 0;

	return strncmp(range->start, str, len) == 0 ? len : 0;
}

String* TextRange_toString(TextRange *range) {
	if(!range) return NULL;

	return String_fromSubstring(range->start, 0, range->end - range->start);
}

TextRange TextRange_construct(char *start, char *end, int line, int column) {
	TextRange range;
	TextRange_constructor(&range, start, end, line, column);
	return range;
}

void TextRange_print_compact(TextRange *range) {
	if(!range) {
		printf("TextRange { NULL }\n");
		return;
	}

	print_type_head("TextRange", "{");
	printf(AQUA "%d:%d:", range->line, range->column);
	print_string(range->start, range->end);
	print_type_tail("}");
}

void TextRange_print(TextRange *range, unsigned int depth, int isProperty) {
	if(!range) {
		print_null_type("Array");
		return;
	}

	// if(!isProperty) indent(depth);
	// printf("TextRange {\n");

	// indent(depth + 1);
	// printf("start: %p\n", range->start);

	// indent(depth + 1);
	// printf("end: %p\n", range->end);

	// indent(depth + 1);
	// printf("line: %d\n", range->line);

	// indent(depth + 1);
	// printf("column: %d\n", range->column);

	// indent(depth + 1);
	// printf("length: %zu\n", range->length);

	// indent(depth + 1);
	// printf("__text__: ");
	// print_string(range->start, range->end);
	// putchar('\n');

	// indent(depth);
	// printf("}\n");

	print_type_begin("TextRange");

	print_field("start", POINTER "%p", range->start);
	print_field("end", POINTER "%p", range->end);
	print_field("line", NUMBER "%d", range->line);
	print_field("column", NUMBER "%d", range->column);
	print_field("length", NUMBER "%zu", range->length);

	print_field("__text__");
	print_string(range->start, range->end);
	putchar('\n');

	print_type_end();
}
