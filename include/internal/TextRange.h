#include <stdlib.h>

#ifndef TEXT_RANGE_H
#define TEXT_RANGE_H

typedef struct {
	char *start;
	char *end;
	int line;
	int column;
	size_t length;
} TextRange;

void TextRange_constructor(TextRange *range, char *start, char *end, int line, int column);
void TextRange_destructor(TextRange *range);
char TextRange_peek(TextRange *range, int offset);
size_t TextRange_compare(TextRange *range, char *str);
String* TextRange_toString(TextRange *range);

TextRange TextRange_construct(char *start, char *end, int line, int column);

void TextRange_print_compact(TextRange *range);
void TextRange_print(TextRange *range, unsigned int depth, int isProperty);

#endif
