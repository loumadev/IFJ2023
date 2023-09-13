#include <string.h>
#include "utils.h"
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

String* String_clone(String *string) {
	if(!string) return NULL;

	return String_alloc(string->value);
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

String* String_fromNumber(long value, char *format) {
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
	int length = snprintf(string->value, 31, format, value);

	string->length = length;
	string->capacity = string->length;

	return string;
}

String* String_fromULong(unsigned long value) {
	return String_fromNumber(value, "%lu");
}

String* String_fromLong(long value) {
	return String_fromNumber(value, "%ld");
}

String* String_fromDouble(double value) {
	return String_fromNumber(value, "%f");
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

void String_print(String *string, unsigned int depth, int isProperty) {
	if(!string) {
		if(!isProperty) indent(depth);
		printf("String { NULL }\n");
		return;
	}

	if(!isProperty) indent(depth);
	printf("String {\n");

	indent(depth + 1);
	printf("value: ");
	if(string->value) print_string(string->value, NULL), putchar('\n');
	else printf("NULL\n");

	indent(depth + 1);
	printf("length: %zu\n", string->length);

	indent(depth + 1);
	printf("capacity: %zu\n", string->capacity);

	indent(depth);
	printf("}\n");
}
