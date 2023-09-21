#include <stdlib.h>
#include <stdbool.h>

#ifndef STRING_H
#define STRING_H

typedef struct String {
	char *value;
	size_t length;
	size_t capacity;
} String;

void String_constructor(String *string, char *value);
void String_destructor(String *string);
void String_setSize(String *string, size_t capacity);
void String_set(String *string, char *value);
void String_append(String *string, char *value);
void String_appendChar(String *string, char value);
bool String_equals(String *string, char *value);
bool String_startsWith(String *string, char *value);
bool String_endsWith(String *string, char *value);
long String_indexOf(String *string, char *value);
void String_copy(String *string, char *dest, size_t length);
void String_splice(String *string, size_t start, size_t end, char *replacement);
String* String_slice(String *string, size_t start, size_t end);
String* String_clone(String *string);
String* String_fromFormat(char *format, ...);
String* String_fromSubstring(char *value, size_t start, size_t end);
String* String_fromNumber(long value, char *format);
String* String_fromULong(unsigned long value);
String* String_fromLong(long value);
String* String_fromDouble(double value);

String* String_alloc(char *value);
void String_free(String *string);

void String_print_compact(String *string);
void String_print(String *string, unsigned int depth, int isProperty);

#endif
