#include <stdarg.h>
#include <string.h>
#include "inspector.h"
#include "internal/String.h"
#include "allocator/MemoryAllocator.h"

void String_constructor(String *string, char *value) {
	if(!string) return;

	string->value = NULL;
	string->length = 0;
	string->capacity = 0;

	// Has protection agains re-constructing already constructed objects
	String_set(string, value);
}

void String_destructor(String *string) {
	if(!string) return;

	if(string->value) mem_free(string->value);
	string->value = NULL;
	string->length = 0;
	string->capacity = 0;
}

// Private
void String_resize(String *string, size_t capacity, bool force) {
	if(!string) return;
	if(!force && capacity <= string->capacity && string->value) return;

	string->value = string->value ? mem_realloc(string->value, capacity) : mem_alloc(capacity);
	if(!string->value) return;

	string->capacity = capacity;
}

void String_setSize(String *string, size_t capacity) {
	String_resize(string, capacity, true);
}

void String_set(String *string, char *value) {
	if(!string) return;
	if(!value) return;

	size_t length = strlen(value);

	// Allocate memory for the string
	String_resize(string, length + 1, true);
	if(!string->value) return;

	// Copy the string
	strcpy(string->value, value);
	string->length = length;
	string->capacity = string->length;
}

void String_append(String *string, char *value) {
	if(!string) return;
	if(!value) return;

	size_t length = strlen(value);
	size_t newLength = string->length + length;

	// Resize the string
	String_resize(string, newLength + 1, false);
	if(!string->value) return;

	// Copy the string to offset
	memcpy(string->value + string->length, value, length);
	string->length = newLength;
	string->value[string->length] = '\0';
}

void String_appendChar(String *string, char value) {
	if(!string) return;

	// Resize the string
	String_resize(string, string->length + 1 + 1, false);
	if(!string->value) return;

	// Append the character
	string->value[string->length] = value;
	string->length++;
	string->value[string->length] = '\0';
}

void String_replaceAll(String *string, char *value, char *replacement) {
	if(!string) return;
	if(!value) return;
	if(!replacement) return;

	size_t valueLength = strlen(value);
	size_t replacementLength = strlen(replacement);

	if(!string->value) return;

	// Replace all occurrences
	long index = 0;
	while((index = String_indexOf(string, value)) != -1) {
		String_splice(string, index, index + valueLength, replacement);
		index += replacementLength;
	}
}

bool String_equals(String *string, char *value) {
	if(!string) return false;
	if(!value) return false;
	if(!string->value) return false;

	return strcmp(string->value, value) == 0;
}

bool String_startsWith(String *string, char *value) {
	if(!string) return false;
	if(!value) return false;
	if(!string->value) return false;

	size_t length = strlen(value);
	if(length > string->length) return false;

	return strncmp(string->value, value, length) == 0;
}

bool String_endsWith(String *string, char *value) {
	if(!string) return false;
	if(!value) return false;
	if(!string->value) return false;

	size_t length = strlen(value);
	if(length > string->length) return false;

	return strcmp(string->value + string->length - length, value) == 0;
}

long String_indexOf(String *string, char *value) {
	if(!string) return -1;
	if(!value) return -1;
	if(!string->value) return -1;

	char *result = strstr(string->value, value);
	if(!result) return -1;

	return result - string->value;
}

void String_copy(String *string, char *dest, size_t length) {
	if(!string) return;
	if(!dest) return;

	// Copy the string
	dest[0] = '\0';
	strncat(dest, string->value, length);
}

void String_splice(String *string, size_t start, size_t end, char *replacement) {
	if(!string) return;
	if(!replacement) return;
	if(!string->value) return;

	if(start > string->length) {
		start = string->length;
		end = string->length;
	}

	if(end > string->length) end = string->length;

	if(start > end) end = start;

	size_t length = strlen(replacement);
	size_t newLength = string->length + length - (end - start);

	// Resize the string
	String_resize(string, newLength + 1, false);
	if(!string->value) return;

	// Move the string
	memmove(string->value + start + length, string->value + end, string->length - end);
	memcpy(string->value + start, replacement, length);
	string->length = newLength;
	string->value[string->length] = '\0';
}

String* String_slice(String *string, size_t start, size_t end) {
	if(!string) return NULL;
	if(start > end) return NULL;
	if(start >= string->length) return NULL;
	if(end > string->length) end = string->length;

	size_t length = end - start;

	// Allocate memory for the string
	String *newString = String_alloc(NULL);
	if(!newString) return NULL;

	// Copy the substring
	String_resize(newString, length + 1, true);
	newString->value[0] = '\0';
	strncat(newString->value, string->value + start, length);

	newString->value[length] = '\0';
	newString->length = length;
	newString->capacity = newString->length;

	return newString;
}

String* String_clone(String *string) {
	if(!string) return NULL;

	return String_alloc(string->value);
}


String* String_fromFormat(char *format, ...) {
	va_list args;

	// Allocate memory for the string
	String *string = String_alloc(NULL);
	if(!string) return NULL;

	// Resize the string
	String_resize(string, 128, true);

	// Gather the arguments and format the string
	va_start(args, format);
	int length = vsnprintf(string->value, 127, format, args);
	va_end(args);

	if(length < 0) return NULL;

	// Resize the string if necessary
	if((size_t)length >= string->capacity) {
		String_resize(string, length + 1, true);

		// Re-gather the arguments and format the string
		va_start(args, format);
		length = vsnprintf(string->value, length, format, args);
		va_end(args);
	}

	string->length = length;
	string->capacity = string->length;

	return string;
}

String* String_fromSubstring(char *value, size_t start, size_t end) {
	if(!value) return NULL;
	if(start > end) return NULL;

	size_t length = end - start;

	// Allocate memory for the string
	String *string = String_alloc(NULL);
	if(!string) return NULL;

	// Copy the substring
	String_resize(string, length + 1, true);
	string->value[0] = '\0';
	strncat(string->value, value + start, length);

	string->value[length] = '\0';
	string->length = length;
	string->capacity = string->length;

	return string;
}

String* String_fromLong(long value) {
	// Allocate memory for the string
	String *string = String_alloc(NULL);
	if(!string) return NULL;

	// Resize the string
	String_resize(string, 32, true);
	if(!string->value) {
		String_free(string);
		return NULL;
	}

	// Convert the long to a string
	int length = snprintf(string->value, 31, "%ld", value);

	string->length = length;
	string->capacity = string->length;

	return string;
}

String* String_fromDouble(double value) {
	// Allocate memory for the string
	String *string = String_alloc(NULL);
	if(!string) return NULL;

	// Resize the string
	String_resize(string, 32, true);
	if(!string->value) {
		String_free(string);
		return NULL;
	}

	// Convert the long to a string
	int length = snprintf(string->value, 31, "%lf", value);

	string->length = length;
	string->capacity = string->length;

	return string;
}


String* String_alloc(char *value) {
	String *string = mem_alloc(sizeof(String));
	if(!string) return NULL;

	String_constructor(string, value);

	return string;
}

void String_free(String *string) {
	if(!string) return;

	String_destructor(string);
	mem_free(string);
}

void String_print_compact(String *string) {
	if(!string) return (void)print_null_nl();

	print_type_head("String", "{");
	print_string(string->value, NULL);
	print_type_tail("}");
}

void String_print(String *string, unsigned int depth, int isProperty) {
	if(!string) {
		print_null_type("String");
		return;
	}

	print_type_begin("String");

	print_field("value");
	if(string->value) print_string(string->value, NULL), putchar('\n');
	else print_null_field();

	print_field("length", NUMBER "%zu", string->length);
	print_field("capacity", NUMBER "%zu", string->capacity);

	print_type_end();
}
