#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "Array.h"

#ifndef STRING_H
#define STRING_H

typedef struct String {
	char *value;
	size_t length;
	size_t capacity;
} String;

/**
 * Initializes a String with the given value.
 *
 * @param string The String to initialize.
 * @param value The value to initialize the String with.
 */
void String_constructor(String *string, char *value);

/**
 * Deallocates the memory used by a String.
 *
 * @param string The String to deallocate.
 */
void String_destructor(String *string);

/**
 * Sets the capacity of a String.
 *
 * @param string The String to set the capacity of.
 * @param capacity The new capacity of the String.
 */
void String_setSize(String *string, size_t capacity);

/**
 * Sets the value of a String.
 *
 * @param string The String to set the value of.
 * @param value The new value of the String.
 */
void String_set(String *string, char *value);

/**
 * Appends a value to the end of a String.
 *
 * @param string The String to append to.
 * @param value The value to append to the String.
 */
void String_append(String *string, char *value);

/**
 * Appends a character to the end of a String.
 *
 * @param string The String to append to.
 * @param value The character to append to the String.
 */
void String_appendChar(String *string, char value);

/**
 * Replaces all occurrences of a `value` in a String with a `replacement` value.
 *
 * @param string The String to replace values in.
 * @param value The value to replace.
 * @param replacement The replacement value.
 */
void String_replaceAll(String *string, char *value, char *replacement);

/**
 * Checks if a String is equal to a given value.
 *
 * @param string The String to compare.
 * @param value The value to compare to.
 * @return true if the String is equal to the value, false otherwise.
 */
bool String_equals(String *string, char *value);

/**
 * Checks if a String starts with a given value.
 *
 * @param string The String to check.
 * @param value The value to check for at the start of the String.
 * @return true if the String starts with the value, false otherwise.
 */
bool String_startsWith(String *string, char *value);

/**
 * Checks if a String ends with a given value.
 *
 * @param string The String to check.
 * @param value The value to check for at the end of the String.
 * @return true if the String ends with the value, false otherwise.
 */
bool String_endsWith(String *string, char *value);

/**
 * Returns the index of the first occurrence of a value in a String.
 *
 * @param string The String to search in.
 * @param value The value to search for.
 * @return The index of the first occurrence of the value, or -1 if not found.
 */
long String_indexOf(String *string, char *value);

/**
 * Returns the character at a given index in a String.
 *
 * @param string The String to get the character from.
 * @param index The index of the character to get. (negative values start from the end)
 * @return The character at the given index.
 */
char String_charAt(String *string, signed long index);

/**
 * Copies the contents of the String to a given buffer of a given length.
 *
 * @param string The String to copy.
 * @param dest The buffer to copy the String to.
 * @param length The length of the buffer.
 */
void String_copy(String *string, char *dest, size_t length);

/**
 * Removes a portion of a String and replaces it with a given value.
 *
 * @param string The String to splice.
 * @param start The starting index of the portion to remove.
 * @param end The ending index of the portion to remove.
 * @param replacement The value to replace the removed portion with.
 */
void String_splice(String *string, size_t start, size_t end, char *replacement);

/**
 * Splits a String into an Array of Strings using a separator.
 *
 * @param string The String to split.
 * @param separator The separator to use.
 */
Array* String_split(String *string, char *separator);

/**
 * Returns a new String created by joining the values of an Array with a separator.
 *
 * @param array The Array of Strings to join.
 * @param separator The separator to use between values.
 * @return A new String created by joining the values of the Array.
 */
String* String_join(Array *array, char *separator);

/**
 * Returns a new String containing a portion of the original String.
 *
 * @param string The String to slice.
 * @param start The starting index of the portion to slice.
 * @param end The ending index of the portion to slice.
 * @return A new String containing the sliced portion.
 */
String* String_slice(String *string, size_t start, size_t end);

/**
 * Returns a new String that is a copy of the original String.
 *
 * @param string The String to clone.
 * @return A new String that is a copy of the original.
 */
String* String_clone(String *string);

/**
 * Returns a new String created from a format string and arguments.
 *
 * @param format The format string to use.
 * @param ... The arguments to use in the format string.
 * @return A new String created from the format string and arguments.
 */
String* String_fromFormat(char *format, ...);

/**
 * Returns a new String created from a portion of a given value.
 *
 * @param value The value to create the new String from.
 * @param start The starting index of the portion to use.
 * @param end The ending index of the portion to use.
 * @return A new String created from the portion of the value.
 */
String* String_fromSubstring(char *value, size_t start, size_t end);

/**
 * Returns a new String created from a given long.
 *
 * @param value The long to create the new String from.
 * @return A new String created from the long.
 */
String* String_fromLong(long value);

/**
 * Returns a new String created from a given double.
 *
 * @param value The double to create the new String from.
 * @return A new String created from the double.
 */
String* String_fromDouble(double value);

/**
 * Returns a new String created from the given range.
 *
 * @param start The starting pointer of the range to use.
 * @param end The ending pointer of the range to use.
 * @return A new String created from the range.
 */
String* String_fromRange(char *start, char *end);

/**
 * Allocates memory for a new String and initializes it with the given value.
 *
 * @param value The value to initialize the new String with.
 * @return A pointer to the newly allocated String.
 */
String* String_alloc(char *value);

/**
 * Deallocates the memory used by a String.
 *
 * @param string The String to deallocate.
 */
void String_free(String *string);

/**
 * Prints the value of a String in a compact format.
 *
 * @param string The String to print.
 */
void String_print_compact(String *string);

/**
 * Prints the value of a String, including its properties.
 *
 * @param string The String to print.
 * @param depth The current depth of the printing. (default: 0) (used internally)
 * @param isProperty Whether the String is a property of a larger object. (default: 0) (used internally)
 */
void String_print(String *string, unsigned int depth, int isProperty);

#endif
