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

/**
 * Initializes a TextRange struct.
 *
 * @param range The TextRange struct to initialize.
 * @param start A pointer to the start of the text range.
 * @param end A pointer to the end of the text range.
 * @param line The line number of the text range.
 * @param column The column number of the text range.
 */
void TextRange_constructor(TextRange *range, char *start, char *end, int line, int column);

/**
 * Deallocates the memory used by a TextRange struct.
 *
 * @param range The TextRange struct to deallocate.
 */
void TextRange_destructor(TextRange *range);

/**
 * Returns the character at the given offset from the start of the text range.
 *
 * @param range The TextRange struct to peek into.
 * @param offset The offset from the start of the text range.
 * @return The character at the given offset, or '\0' if the offset is out of bounds.
 */
char TextRange_peek(TextRange *range, int offset);

/**
 * Compares the text range to a given string.
 *
 * @param range The TextRange struct to compare.
 * @param str The string to compare to.
 * @return Length of the `str` if the text range starts with the `str`, otherwise 0.
 */
size_t TextRange_compare(TextRange *range, char *str);

/**
 * Returns a string representation of the text range.
 *
 * @param range The TextRange struct to convert to a string.
 * @return A String struct containing the text range.
 */
String* TextRange_toString(TextRange *range);

/**
 * Constructs a new TextRange struct.
 *
 * @param start A pointer to the start of the text range.
 * @param end A pointer to the end of the text range.
 * @param line The line number of the text range.
 * @param column The column number of the text range.
 * @return A new TextRange struct.
 */
TextRange TextRange_construct(char *start, char *end, int line, int column);

/**
 * Prints the text range in a compact format.
 *
 * @param range The TextRange struct to print.
 */
void TextRange_print_compact(TextRange *range);

/**
 * Prints the text range, including its properties.
 *
 * @param range The TextRange struct to print.
 * @param depth The current depth of the printing. (default: 0) (used internally)
 * @param isProperty Whether the text range is a property of a larger object. (default: 0) (used internally)
 */
void TextRange_print(TextRange *range, unsigned int depth, int isProperty);

#endif
