#include <stdbool.h>
#include <string.h>

#include "compiler/lexer/Lexer.h"
#include "internal/String.h"
#include "internal/Array.h"
#include "inspector.h"
#include "assertf.h"

bool __Lexer_resolveEscapedChar(char ch, char *out);

LexerResult __Lexer_tokenizeWhitespace(Lexer *lexer);
LexerResult __Lexer_tokenizeSpace(Lexer *lexer);
LexerResult __Lexer_tokenizeNewLine(Lexer *lexer);
LexerResult __Lexer_tokenizeSingleLineComment(Lexer *lexer);
LexerResult __Lexer_tokenizeMultiLineComment(Lexer *lexer);

LexerResult __Lexer_tokenizeString(Lexer *lexer);
LexerResult __Lexer_tokenizeIdentifier(Lexer *lexer);
LexerResult __Lexer_tokenizeNumberLiteral(Lexer *lexer);
LexerResult __Lexer_tokenizeIntegerBasedLiteral(Lexer *lexer, int base);
LexerResult __Lexer_tokenizeBinaryLiteral(Lexer *lexer);
LexerResult __Lexer_tokenizeOctalLiteral(Lexer *lexer);
LexerResult __Lexer_tokenizeHexadecimalLiteral(Lexer *lexer);
LexerResult __Lexer_tokenizeDecimalLiteral(Lexer *lexer);
LexerResult __Lexer_tokenizePunctuatorsAndOperators(Lexer *lexer);

void Lexer_constructor(Lexer *lexer) {
	if(!lexer) return;

	lexer->source = NULL;
	lexer->sourceLength = 0;
	lexer->tokens = Array_alloc(2);
	lexer->currentTokenIndex = -1;
	lexer->currentChar = NULL;
	lexer->line = 1;
	lexer->column = 1;
	lexer->whitespace = WHITESPACE_NONE;
}

void Lexer_destructor(Lexer *lexer) {
	if(!lexer) return;

	if(lexer->tokens) {
		for(size_t i = 0; i < lexer->tokens->size; i++) {
			Token_destructor((Token*)Array_get(lexer->tokens, i));
		}

		Array_destructor(lexer->tokens);
		lexer->tokens = NULL;
	}

	lexer->source = NULL;
	lexer->sourceLength = 0;
	lexer->currentChar = NULL;
	lexer->currentTokenIndex = -1;
	lexer->line = 0;
	lexer->column = 0;
	lexer->whitespace = WHITESPACE_NONE;
}

char Lexer_peekChar(Lexer *lexer, size_t offset) {
	if(!lexer) return '\0';
	if(!lexer->currentChar) return '\0';
	if(lexer->currentChar + offset >= lexer->source + lexer->sourceLength) return '\0';

	return *(lexer->currentChar + offset);
}

size_t Lexer_compare(Lexer *lexer, char *str) {
	if(!lexer) return 0;
	if(!str) return 0;

	size_t length = strlen(str);

	if(strncmp(lexer->currentChar, str, length) != 0) return 0;

	return length;
}

// TODO: Test this at the end of the input (may overflow to the beginning of the string for some reason)
size_t Lexer_match(Lexer *lexer, char *str) {
	if(!lexer) return 0;
	if(!str) return 0;

	size_t offset = Lexer_compare(lexer, str);
	if(offset == 0) return 0;

	lexer->currentChar += offset;
	return offset;
}

char Lexer_advance(Lexer *lexer) {
	if(!lexer) return '\0';
	if(!lexer->currentChar) return '\0';

	char ch = *++lexer->currentChar;
	// Prevent from advancing past the end of the string
	// if(ch == '\0') lexer->currentChar--;

	return ch;
}

bool Lexer_isAtEnd(Lexer *lexer) {
	if(!lexer) return true;
	if(!lexer->currentChar) return true;

	return lexer->currentChar >= lexer->source + lexer->sourceLength;
}

#define is_space_like(ch) ((ch) == ' ' || (ch) == '\f' || (ch) == '\t' /*|| (ch) == '\v'*/)
#define is_newline(ch) ((ch) == '\n' || (ch) == '\r')
#define is_whitespace(ch) (is_space_like(ch) || is_newline(ch))
#define is_single_line_comment(lexer) (Lexer_compare(lexer, "//") != 0)
#define is_multi_line_comment_start(lexer) (Lexer_compare(lexer, "/*") != 0)
#define is_multi_line_comment_end(lexer) (Lexer_compare(lexer, "*/") != 0)
#define is_multi_line_comment(lexer) (is_multi_line_comment_start(lexer) || is_multi_line_comment_end(lexer))
#define is_comment(lexer) (is_single_line_comment(lexer) || is_multi_line_comment(lexer))

#define is_alpha(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define is_binary_digit(ch) ((ch) == '0' || (ch) == '1')
#define is_octal_digit(ch) ((ch) >= '0' && (ch) <= '7')
#define is_decimal_digit(ch) ((ch) >= '0' && (ch) <= '9')
#define is_hexadecimal_digit(ch) (((ch) >= '0' && (ch) <= '9') || ((ch) >= 'a' && (ch) <= 'f') || ((ch) >= 'A' && (ch) <= 'F'))
#define is_base_digit(ch, base) (base == 2 ? is_binary_digit(ch) : base == 8 ? is_octal_digit(ch) : base == 16 ? is_hexadecimal_digit(ch) : is_decimal_digit(ch))
#define is_identifier_start(ch) ((ch) == '_' || is_alpha(ch))
#define is_identifier_part(ch) (is_identifier_start(ch) || is_decimal_digit(ch))

#define fetch_next_whitespace(lexer) enum WhitespaceType __wh_bit = lexer->whitespace; LexerResult __wh_res = __Lexer_tokenizeWhitespace(lexer); if(!__wh_res.success) return __wh_res; __wh_bit |= left_to_right_whitespace(lexer->whitespace);

// TODO: Move this somewhere else (to some utility file)
int max(int a, int b) {
	return a > b ? a : b;
}

LexerResult __Lexer_tokenizeWhitespace(Lexer *lexer) {
	if(!lexer) return LexerNoMatch();
	if(!lexer->currentChar) return LexerNoMatch();

	// enum WhitespaceType prev = lexer->whitespace;       // Save previous ws in case of no match

	lexer->whitespace = WHITESPACE_NONE;                // Prepare ws for matching
	enum WhitespaceType whitespace = WHITESPACE_NONE;   // Keep track of the matched whitespace

	bool matched = false;
	bool hasMatch = false;
	LexerResult res;

	do {
		matched = false;

		res = __Lexer_tokenizeSpace(lexer);
		if(!res.success) return res;
		if(res.type != RESULT_NO_MATCH) whitespace = max(whitespace, lexer->whitespace & WHITESPACE_MASK_LEFT), matched = true;
		// TODO: Free the results here

		res = __Lexer_tokenizeNewLine(lexer);
		if(!res.success) return res;
		if(res.type != RESULT_NO_MATCH) whitespace = max(whitespace, lexer->whitespace & WHITESPACE_MASK_LEFT), matched = true;

		res = __Lexer_tokenizeSingleLineComment(lexer);
		if(!res.success) return res;
		if(res.type != RESULT_NO_MATCH) whitespace = max(whitespace, lexer->whitespace & WHITESPACE_MASK_LEFT), matched = true;

		res = __Lexer_tokenizeMultiLineComment(lexer);
		if(!res.success) return res;
		if(res.type != RESULT_NO_MATCH) whitespace = max(whitespace, lexer->whitespace & WHITESPACE_MASK_LEFT), matched = true;

		hasMatch |= matched;
	} while(matched);

	// lexer->whitespace = hasMatch ? whitespace : prev;
	lexer->whitespace = whitespace;

	return hasMatch ? LexerSuccess() : LexerNoMatch();
}

LexerResult __Lexer_tokenizeSpace(Lexer *lexer) {
	if(!lexer) return LexerNoMatch();
	if(!lexer->currentChar) return LexerNoMatch();

	char ch = *lexer->currentChar;

	if(!is_space_like(ch)) return LexerNoMatch();

	while((ch = *lexer->currentChar) && is_space_like(ch)) {
		Lexer_advance(lexer);
	}

	lexer->whitespace = WHITESPACE_LEFT_SPACE;

	return LexerSuccess();
}

LexerResult __Lexer_tokenizeNewLine(Lexer *lexer) {
	if(!lexer) return LexerNoMatch();
	if(!lexer->currentChar) return LexerNoMatch();

	char ch = *lexer->currentChar;

	if(!is_newline(ch)) return LexerNoMatch();

	while((ch = *lexer->currentChar) && is_newline(ch)) {
		Lexer_advance(lexer);
	}

	lexer->whitespace = WHITESPACE_LEFT_NEWLINE;

	return LexerSuccess();
}

LexerResult __Lexer_tokenizeSingleLineComment(Lexer *lexer) {
	if(!lexer) return LexerNoMatch();
	if(!lexer->currentChar) return LexerNoMatch();

	if(!is_single_line_comment(lexer)) return LexerNoMatch();

	while(!Lexer_isAtEnd(lexer) && !Lexer_match(lexer, "\n")) {
		Lexer_advance(lexer);
	}

	lexer->whitespace = WHITESPACE_LEFT_NEWLINE;

	return LexerSuccess();
}

LexerResult __Lexer_tokenizeMultiLineComment(Lexer *lexer) {
	if(!lexer) return LexerNoMatch();
	if(!lexer->currentChar) return LexerNoMatch();

	if(!is_multi_line_comment(lexer)) return LexerNoMatch();
	if(is_multi_line_comment_end(lexer)) return LexerError(
			String_fromFormat("unexpected end of block comment"),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	enum WhitespaceType whitespace = WHITESPACE_LEFT_SPACE;
	size_t depth = 0;

	while(!Lexer_isAtEnd(lexer)) {
		if(Lexer_match(lexer, "/*")) {
			depth++;
		} else if(Lexer_match(lexer, "*/")) {
			depth--;

			if(depth == 0) break;
		} else {
			char ch = Lexer_advance(lexer);
			if(is_newline(ch)) whitespace = WHITESPACE_LEFT_NEWLINE;
		}
	}

	// There are still comments left
	if(depth != 0) return LexerError(
			String_fromFormat("unterminated '/*' comment"),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	lexer->whitespace = whitespace;

	return LexerSuccess();
}


void Lexer_setSource(Lexer *lexer, char *source) {
	assertf(lexer != NULL);

	Lexer_destructor(lexer);
	Lexer_constructor(lexer);

	lexer->line = 1;
	lexer->column = 1;
	lexer->source = source;
	lexer->sourceLength = strlen(source);
	lexer->currentChar = lexer->source;
}

Token* Lexer_getUpcomingToken(Lexer *lexer) {
	assertf(lexer != NULL);

	Token *token = Array_get(lexer->tokens, lexer->currentTokenIndex + 1);
	if(!token) return NULL;

	// Move to the next token
	lexer->currentTokenIndex++;

	return token;
}

LexerResult Lexer_tokenizeNextToken(Lexer *lexer) {
	assertf(lexer != NULL);

	char ch = *lexer->currentChar;

	// Update line and column counters
	if(ch == '\n') {
		lexer->line++;
		lexer->column = 1;
	} else {
		lexer->column++;
	}

	// Skip whitespace
	{
		enum WhitespaceType prev = lexer->whitespace;               // Save previous ws in case of no match

		LexerResult res = __Lexer_tokenizeWhitespace(lexer);
		if(!res.success) return res;

		// Some whitespace matched
		if(res.type != RESULT_NO_MATCH) return Lexer_tokenizeNextToken(lexer);

		lexer->whitespace = prev;
	}

	// EOF
	if(ch == '\0') {
		// Create an EOF token
		TextRange range;
		TextRange_constructor(&range, lexer->currentChar, lexer->currentChar, lexer->line, lexer->column);

		fetch_next_whitespace(lexer);

		Token *token = Token_alloc(TOKEN_EOF, TOKEN_DEFAULT, __wh_bit, range, (union TokenValue){0});
		assertf(token != NULL);

		Array_push(lexer->tokens, token);

		return LexerSuccess();
	}
	// Match string literals
	// TODO: Allow only double-quoted string literals
	else if(ch == '"' || ch == '\'') {
		return __Lexer_tokenizeString(lexer);
	}
	// Match identifiers
	else if(is_identifier_start(ch)) {
		return __Lexer_tokenizeIdentifier(lexer);
	}
	// Match numbers
	else if(
		is_decimal_digit(ch)// ||
		// (ch == '.' && is_decimal_digit(Lexer_peekChar(lexer, 1)))         // Error state
	) {
		return __Lexer_tokenizeNumberLiteral(lexer);
	}
	// Something other
	else {
		// Try to match punctuators and operators
		LexerResult result = __Lexer_tokenizePunctuatorsAndOperators(lexer);
		if(result.success && result.type != RESULT_NO_MATCH) return result;

		// Invalid character
		return LexerError(
			String_fromFormat("unexpected token '%s'", format_char(ch)),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
		);
	}
}

LexerResult Lexer_nextToken(Lexer *lexer) {
	assertf(lexer != NULL);
	assertf(lexer->source != NULL, "Cannot process the next token: No source set");

	// If there are already processed tokens, return the next one
	Token *cachedToken = Lexer_getUpcomingToken(lexer);
	if(cachedToken) {
		LexerResult result = LexerSuccess();
		result.token = cachedToken;
		return result;
	}

	// Otherwise tokenize the next token
	LexerResult result = Lexer_tokenizeNextToken(lexer);
	if(!result.success) return result;

	// Return the result with token
	result.token = Lexer_getUpcomingToken(lexer);
	if(!result.token) {
		warnf("The tokenization resulted in no tokens, trying to tokenize the next one");
		return Lexer_nextToken(lexer);
	}

	return result;
}

LexerResult Lexer_peekToken(Lexer *lexer, int offset) {
	assertf(lexer != NULL);

	int index = lexer->currentTokenIndex + offset;

	// If there are no tokens yet, move the index to the first one
	// EDIT: This is would always skip the first token, which is incorrect
	// if(lexer->currentTokenIndex == -1) index++;

	// Peeking before the start of the token stream
	if(index < 0) {
		LexerResult result = LexerSuccess();
		result.token = NULL;
		return result;
	}

	// Peeking before the end of the token stream
	if(offset < 0) {
		LexerResult result = LexerSuccess();
		result.token = Array_get(lexer->tokens, index);
		return result;
	}

	// Peeking at the end of the token stream (or after the token stream)
	Token *token = Array_get(lexer->tokens, index);
	if(token) {
		LexerResult result = LexerSuccess();
		result.token = token;
		return result;
	}

	// Save current index for backtracking (since peeking should not consume the tokens)
	size_t backtrack = lexer->currentTokenIndex;

	// Peeking after the end of the token stream
	LexerResult result = LexerSuccess();
	while((result = Lexer_nextToken(lexer)).token && result.token->type != TOKEN_EOF) {
		// If the target token is found, return it
		if(lexer->currentTokenIndex == index) return (lexer->currentTokenIndex = backtrack), result;
	}

	lexer->currentTokenIndex = backtrack;

	// Exit if something fails
	if(!result.success) return result;

	// This will be always an EOF token
	return result;
}

LexerResult Lexer_tokenize(Lexer *lexer, char *source) {
	assertf(lexer != NULL);

	// Reset the lexer
	Lexer_destructor(lexer);
	Lexer_constructor(lexer);

	// Set the source to tokenize
	Lexer_setSource(lexer, source);

	// While there are tokens to process
	LexerResult result = LexerSuccess();
	while((result = Lexer_nextToken(lexer)).token && result.token->type != TOKEN_EOF) {
		// Do nothing, just drain the source stream to get all the tokens
	}

	// Exit if something fails
	if(!result.success) return result;

	return LexerSuccess();
}

LexerResult __Lexer_tokenizeUntilStringInterpolationTerminator(Lexer *lexer) {
	if(!lexer) return LexerNoMatch();
	if(!lexer->currentChar) return LexerNoMatch();

	// Keep track of the depth in case of nested parentheses
	// 1 - the initial string interpolation parenthesis to match with the closing one
	size_t depth = 1;

	// While there are tokens to process
	LexerResult result = LexerSuccess();
	// while((result = Lexer_tokenizeNextToken(lexer)).token && result.token->type != TOKEN_EOF) {
	// 	if(result.token->kind == TOKEN_LEFT_PAREN) {
	// 		depth++;
	// 	} else if(result.token->kind == TOKEN_RIGHT_PAREN) {
	// 		depth--;

	// 		if(depth == 0) break;
	// 	}
	// }

	do {
		result = Lexer_tokenizeNextToken(lexer);
		if(!result.success) return result;

		// Get the token at the top of the token stream
		Token *token = Array_get(lexer->tokens, -1);

		// If the token is an EOF, the interpolation is not terminated
		if(token->type == TOKEN_EOF) return LexerError(
				String_fromFormat("cannot find ')' to match opening '(' in string interpolation"),
				Array_fromArgs(
					1,
					Token_alloc(
						TOKEN_MARKER,
						TOKEN_CARET,
						WHITESPACE_NONE,
						TextRange_construct(
							lexer->currentChar - 1,
							lexer->currentChar,
							lexer->line,
							lexer->column
						),
						(union TokenValue){0}
					)
				)
		);

		// Sort out the parentheses
		// We must NOT consume the closing parentheses, because of potentionally consuming
		// whitespace directly after the closing parenthesis which is part of the string,
		// so instead we will just check for the current char
		if(*lexer->currentChar == '(') {
			depth++;
		} else if(*lexer->currentChar == ')') {
			depth--;
		}
	} while(depth != 0);

	// Consume the closing parenthesis
	Lexer_advance(lexer);

	return LexerSuccess();
}

bool __Lexer_resolveEscapedChar(char ch, char *out) {
	switch(ch) {
		case '0': *out = '\0'; break;
		case 'n': *out = '\n'; break;
		case 'r': *out = '\r'; break;
		case 't': *out = '\t'; break;
		case '\\': *out = '\\'; break;
		case '\'': *out = '\''; break;
		case '"': *out = '"'; break;
		default: return false;
	}

	return true;
}

LexerResult __Lexer_parseUnicodeEscapeSequence(Lexer *lexer, char *out) {
	// Consume the opening brace
	if(!Lexer_match(lexer, "{")) return LexerError(
			String_fromFormat("expected hexadecimal code in braces after unicode escape"),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	// Parse the hexadecimal code
	char *start = lexer->currentChar;
	char *end = NULL;
	long code = strtol(lexer->currentChar, &end, 16);

	// Check if the code is valid
	if(!end || code < 0 || code > 0x10FFFF) return LexerError(
			String_fromFormat("invalid unicode scalar '%d'", code),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						start,
						end,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	// Check for length of the code
	size_t length = end - lexer->currentChar;
	if(length < 1 || length > 8) return LexerError(
			String_fromFormat("\\u{...} escape sequence expects between 1 and 8 hex digits"),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						start,
						end,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	// Consume the hexadecimal code
	lexer->currentChar = end;

	// Check for the closing brace
	if(!Lexer_match(lexer, "}")) return LexerError(
			String_fromFormat("expected closing brace '}' after unicode escape"),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	// Write the code to the output
	// NOTICE: This will only work for values in range 0-255
	*out = code;

	return LexerSuccess();
}

// Swift multiline string literals
// LexerResult __Lexer_tokenizeMultilineStringLiteral(Lexer *lexer) {
// 	char *start = lexer->currentChar;
// 	char ch = *lexer->currentChar;

// 	// Asserts and consumes the string literal start
// 	assertf(Lexer_match(lexer, "\"\"\""), "Unexpected character '%s' (expected '\"' at the source stream head)", format_char(ch));

// 	// Check and consume the LF
// 	if(!Lexer_advance(lexer) != '\n') return LexerError(
// 			String_fromFormat("multi-line string literal content must begin on a new line"),
// 			Token_alloc(
// 				TOKEN_MARKER,
// 				TOKEN_CARET,
// 				WHITESPACE_NONE,
// 				TextRange_construct(
// 					lexer->currentChar,
// 					lexer->currentChar + 1,
// 					lexer->line,
// 					lexer->column
// 				),
// 				(union TokenValue){0}
// 			)
// 	);

// 	Array *tokens = Array_alloc(1);

// 	// Read the string content
// 	// TODO: Tokenize the whole string at once (with interpolation), push the tokens into some internal cache and return the first one, then when calling next(), return the next token from the cache until it's empty, then tokenize the input string.
// 	// TODO: After all the string tokens are collected, process them (remove heading indents)
// }

// TODO: Implement string parsing from specifiaction
LexerResult __Lexer_tokenizeString(Lexer *lexer) {
	char *start = lexer->currentChar;
	char ch = *lexer->currentChar;

	assertf(ch == '"' || ch == '\'', "Unexpected character '%s' (expected '\"' or \"'\" at the source stream head)", format_char(ch));

	char quote = ch;
	ch = Lexer_advance(lexer); // Consume the first character

	String *string = String_alloc("");

	// Match string
	while(ch != quote) {
		// Handle unterminated string literals
		if(ch == '\0' || ch == '\n') return LexerError(
				String_fromFormat("unterminated string literal"),
				Array_fromArgs(
					1,
					Token_alloc(
						TOKEN_MARKER,
						TOKEN_CARET,
						WHITESPACE_NONE,
						TextRange_construct(
							start,
							start + 1,
							lexer->line,
							lexer->column
						),
						(union TokenValue){0}
					)
				)
		);

		// TODO: Handle multiline string literals

		// This consumes two characters, so in case of `\<quote>`, both backslash
		// and the quote are consumed and therefore the loop will not terminate
		if(ch == '\\') {
			// Get the character to escape (consume the backslash)
			char toEscape = Lexer_advance(lexer);

			// Pick the escaping strategy
			if(toEscape == 'u') {
				// TODO: Add support for unicode and hex escapes according to the language specification
				// Consume the 'u'
				Lexer_advance(lexer);

				char escaped = '\0';
				LexerResult res = __Lexer_parseUnicodeEscapeSequence(lexer, &escaped);

				if(!res.success) return res;

				String_appendChar(string, escaped);
				lexer->currentChar--; // Go back one character
			} else if(toEscape == '(') {
				// TODO: Add escape sequences for interpolation

				// Finish the current string literal
				{
					TextRange range;
					TextRange_constructor(&range, start, lexer->currentChar, lexer->line, lexer->column);

					fetch_next_whitespace(lexer);

					// Create a token
					Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_STRING, __wh_bit, range, (union TokenValue){.string = string});
					assertf(token != NULL);

					// Add the token to the array
					Array_push(lexer->tokens, token);
				}

				// Add string interpolation marker
				{
					TextRange range;
					TextRange_constructor(&range, lexer->currentChar, lexer->currentChar, lexer->line, lexer->column);

					fetch_next_whitespace(lexer);

					// Create a token
					Token *token = Token_alloc(TOKEN_STRING_INTERPOLATION_MARKER, TOKEN_STRING_HEAD, __wh_bit, range, (union TokenValue){0});
					assertf(token != NULL);

					// Add the token to the array
					Array_push(lexer->tokens, token);
				}

				// Tokenize the interpolated expression
				{
					// Consume the opening paren
					Lexer_advance(lexer);

					LexerResult res = __Lexer_tokenizeUntilStringInterpolationTerminator(lexer);
					if(!res.success) return res;

					lexer->currentChar--; // Go back one character
				}

				// Add string interpolation marker
				{
					TextRange range;
					TextRange_constructor(&range, lexer->currentChar, lexer->currentChar, lexer->line, lexer->column);

					fetch_next_whitespace(lexer);

					// Create a token
					Token *token = Token_alloc(TOKEN_STRING_INTERPOLATION_MARKER, TOKEN_STRING_SPAN, __wh_bit, range, (union TokenValue){0});
					assertf(token != NULL);

					// Add the token to the array
					Array_push(lexer->tokens, token);
				}

				// Start a new string literal
				{
					start = lexer->currentChar + 1; // +1 to fix the text range
					string = String_alloc("");
				}
			} else {
				char escaped = '\0';
				bool res = __Lexer_resolveEscapedChar(toEscape, &escaped);

				if(!res) return LexerError(
						String_fromFormat("invalid escape sequence '\\%s' in literal", format_char(toEscape)),
						Array_fromArgs(
							1,
							Token_alloc(
								TOKEN_MARKER,
								TOKEN_CARET,
								WHITESPACE_NONE,
								TextRange_construct(
									lexer->currentChar - 1,
									lexer->currentChar,
									lexer->line,
									lexer->column
								),
								(union TokenValue){0}
							)
						)
				);

				String_appendChar(string, escaped);
			}
		} else {
			String_appendChar(string, ch);
		}

		ch = Lexer_advance(lexer);
	}
	// if(ch) lexer->currentChar--;

	// In case the string contains the interpolation, mark the last span marker as tail
	Token *marker = Array_get(lexer->tokens, -1);
	if(marker && marker->kind == TOKEN_STRING_SPAN) marker->kind = TOKEN_STRING_TAIL;

	// Consume the closing quote
	Lexer_advance(lexer);

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar + 1, lexer->line, lexer->column);

	fetch_next_whitespace(lexer);

	// Create a token
	Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_STRING, __wh_bit, range, (union TokenValue){.string = string});
	assertf(token != NULL);

	// Add the token to the array
	Array_push(lexer->tokens, token);
	return LexerSuccess();
}

LexerResult __Lexer_tokenizeIdentifier(Lexer *lexer) {
	char *start = lexer->currentChar;
	char ch = *lexer->currentChar;

	assertf(is_identifier_start(ch), "Unexpected character '%s' (expected identifier start at the source stream head)", format_char(ch));

	// Match identifier
	while(is_identifier_part(ch)) {
		ch = Lexer_advance(lexer);
	}
	// if(ch) lexer->currentChar--; //?

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar, lexer->line, lexer->column);

	// Resolve the identifier
	union TokenValue value = {0};
	enum TokenType type = TOKEN_INVALID;
	enum TokenKind kind = TOKEN_DEFAULT;

	// Look for keywords
	if(TextRange_compare(&range, "true")) type = TOKEN_LITERAL, kind = TOKEN_BOOLEAN, value.boolean = true;
	else if(TextRange_compare(&range, "false")) type = TOKEN_LITERAL, kind = TOKEN_BOOLEAN, value.boolean = false;
	else if(TextRange_compare(&range, "nil")) type = TOKEN_LITERAL, kind = TOKEN_NIL;

	else if(TextRange_compare(&range, "if")) kind = TOKEN_IF;
	else if(TextRange_compare(&range, "else")) kind = TOKEN_ELSE;
	else if(TextRange_compare(&range, "var")) kind = TOKEN_VAR;
	else if(TextRange_compare(&range, "let")) kind = TOKEN_LET;
	else if(TextRange_compare(&range, "while")) kind = TOKEN_WHILE;
	else if(TextRange_compare(&range, "func")) kind = TOKEN_FUNC;
	else if(TextRange_compare(&range, "return")) kind = TOKEN_RETURN;

	// else if(TextRange_compare(&range, "class")) kind = TOKEN_CLASS;
	// else if(TextRange_compare(&range, "else")) kind = TOKEN_ELSE;
	// else if(TextRange_compare(&range, "false")) kind = TOKEN_FALSE;
	// else if(TextRange_compare(&range, "for")) kind = TOKEN_FOR;
	// else if(TextRange_compare(&range, "fun")) kind = TOKEN_FUN;
	// else if(TextRange_compare(&range, "if")) kind = TOKEN_IF;
	// else if(TextRange_compare(&range, "null")) kind = TOKEN_NULL;
	// else if(TextRange_compare(&range, "return")) kind = TOKEN_RETURN;
	// else if(TextRange_compare(&range, "super")) kind = TOKEN_SUPER;
	// else if(TextRange_compare(&range, "this")) kind = TOKEN_THIS;
	// else if(TextRange_compare(&range, "true")) kind = TOKEN_TRUE;
	// else if(TextRange_compare(&range, "var")) kind = TOKEN_VAR;
	// else if(TextRange_compare(&range, "while")) kind = TOKEN_WHILE;

	// If just a regular keyword (without value) or an identifier encountered, set its value to the identifier string
	if(type == TOKEN_INVALID) {
		// Copy the identifier
		String *identifier = TextRange_toString(&range);
		assertf(identifier != NULL);

		// Set a token value
		value.identifier = identifier;
	}

	fetch_next_whitespace(lexer);

	// Create a token
	Token *token = Token_alloc(type == TOKEN_INVALID ? kind == TOKEN_DEFAULT ? TOKEN_IDENTIFIER : TOKEN_KEYWORD : type, kind, __wh_bit, range, value);
	assertf(token != NULL);

	// Add the token to the array
	Array_push(lexer->tokens, token);
	return LexerSuccess();
}

LexerResult __Lexer_tokenizeNumberLiteral(Lexer *lexer) {
	if(Lexer_compare(lexer, "0b")) return __Lexer_tokenizeBinaryLiteral(lexer);
	if(Lexer_compare(lexer, "0o")) return __Lexer_tokenizeOctalLiteral(lexer);
	if(Lexer_compare(lexer, "0x")) return __Lexer_tokenizeHexadecimalLiteral(lexer);

	return __Lexer_tokenizeDecimalLiteral(lexer);
}

LexerResult __Lexer_tokenizeIntegerBasedLiteral(Lexer *lexer, int base) {
	char *prefix =
	base == 2 ? "0b" :
		base == 8 ? "0o" :
			base == 16 ? "0x" :
				NULL;

	assertf(prefix != NULL, "Invalid base '%d'; Allowed bases are 2, 8 and 16", base);

	char *start = lexer->currentChar;

	// Check and consume the prefix
	assertf(Lexer_match(lexer, prefix) != 0, "Expected '%s' at the source stream head", prefix);

	char ch = *lexer->currentChar;
	if(!is_base_digit(ch, base)) return LexerError(
			String_fromFormat(
				"'%s' is not a valid %s in integer literal", format_char(ch),
				base == 2 ? "binary digit (0 or 1)" :
					base == 8 ? "octal digit (0-7)" :
						base == 16 ? "hexadecimal digit (0-9, a-f)" : "digit"
			),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
	);

	// Match number
	while(is_base_digit(*lexer->currentChar, base) || ch == '_') {
		ch = Lexer_advance(lexer);
	}
	// if(ch) lexer->currentChar--; //?

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar, lexer->line, lexer->column);

	// Copy the number
	String *numberStr = TextRange_toString(&range);
	assertf(numberStr != NULL);

	// Remove underscores
	String_replaceAll(numberStr, "_", "");

	// Parse the number and free the string
	long number = strtol(numberStr->value + 2, NULL, base);
	String_free(numberStr);

	fetch_next_whitespace(lexer);

	// Create a token
	Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_INTEGER, __wh_bit, range, (union TokenValue){.integer = number});

	// Add the token to the array
	Array_push(lexer->tokens, token);
	return LexerSuccess();
}

LexerResult __Lexer_tokenizeBinaryLiteral(Lexer *lexer) {
	return __Lexer_tokenizeIntegerBasedLiteral(lexer, 2);
}

LexerResult __Lexer_tokenizeOctalLiteral(Lexer *lexer) {
	return __Lexer_tokenizeIntegerBasedLiteral(lexer, 8);
}

LexerResult __Lexer_tokenizeHexadecimalLiteral(Lexer *lexer) {
	// TODO: Currently not supporting floating point hexadecimal literals
	return __Lexer_tokenizeIntegerBasedLiteral(lexer, 16);
}

LexerResult __Lexer_tokenizeDecimalLiteral(Lexer *lexer) {
	char *start = lexer->currentChar;
	char ch = *lexer->currentChar;

	assertf(is_decimal_digit(ch) || ch == '.', "Unexpected character '%s' (expected decimal digit at the source stream head)", format_char(ch));

	// Match number
	// TODO: Add support for other bases, exponents and checks like '.5' and '075' according to the language specification; Update: none of these are supported
	// Leading zeros are allowed
	bool hasDot = false;
	bool hasExponent = false;

	// TODO: Test "10.test()"
	// TODO: Add handling for leading dot
	// 10 				=> [10]
	// 10.5 			=> [10.5]
	// 10.field 		=> [10, ., "field"]
	// 10.5.field		=> [10.5, ., "field"]
	// 10field			=> [number:LexerError('f' is not a valid digit in integer literal)]
	// 10.5.0field		=> [10.5, ., number:LexerError('f' is not a valid digit in integer literal)]
	// 10.5.0			=> [10.5, ., 0] 				=> [10.5, ., ParserError(expected named member of <...>)]
	// 10.5.0.field		=> [10.5, ., 0, ., "field"]	 	=> [10.5, ., ParseError(expected named member of <...>)]
	// 10.5field		=> [10.5, "field"] 				=> [10.5, ParseError(consecutive statements on a line must be separated by ';')]
	while(is_decimal_digit(ch) || ch == '.' || ch == '_') {
		// Handle fractional part
		if(ch == '.') {
			// '...' and '..<' operators
			if(Lexer_compare(lexer, "...") || Lexer_compare(lexer, "..<")) break;

			// Accessor (ex. 10.123.toFixed())
			if(hasDot) {
				// break;
				return LexerError(
					String_fromFormat("number literal can only contain one floating point dot '.'"),
					Array_fromArgs(
						1,
						Token_alloc(
							TOKEN_MARKER,
							TOKEN_CARET,
							WHITESPACE_NONE,
							TextRange_construct(
								lexer->currentChar,
								lexer->currentChar + 1,
								lexer->line,
								lexer->column
							),
							(union TokenValue){0}
						)
					)
				);
			}
			if(is_identifier_start(Lexer_peekChar(lexer, 1))) break;     // Accessor (ex. 10.toFixed())
			if(!is_decimal_digit(Lexer_peekChar(lexer, 1))) return LexerErrorCustom(
					RESULT_ERROR_SYNTACTIC_ANALYSIS,
					String_fromFormat("expected member name following '.'"),
					Array_fromArgs(
						1,
						Token_alloc(
							TOKEN_MARKER,
							TOKEN_CARET,
							WHITESPACE_NONE,
							TextRange_construct(
								lexer->currentChar,
								lexer->currentChar + 1,
								lexer->line,
								lexer->column
							),
							(union TokenValue){0}
						)
					)
			);

			hasDot = true;
		}

		// Advance to the next character
		ch = Lexer_advance(lexer);

		// Pre-handle exponent
		if((ch == 'e' || ch == 'E') && !hasExponent) {
			hasExponent = true;
			ch = Lexer_advance(lexer);  // Consume the exponent character

			if(ch == '+' || ch == '-') ch = Lexer_advance(lexer);       // Consume the sign character if present

			// Missing exponent
			if(!is_decimal_digit(ch)) return LexerErrorCustom(
					RESULT_ERROR_SYNTACTIC_ANALYSIS,
					String_fromFormat("expected a digit in floating point exponent"),
					Array_fromArgs(
						1,
						Token_alloc(
							TOKEN_MARKER,
							TOKEN_CARET,
							WHITESPACE_NONE,
							TextRange_construct(
								lexer->currentChar,
								lexer->currentChar + 1,
								lexer->line,
								lexer->column
							),
							(union TokenValue){0}
						)
					)
			);
		}

		// Handle invalid characters
		if(!is_alpha(ch)) continue;

		// Invalid character in exponent
		if(!is_decimal_digit(ch) && hasExponent) {
			return LexerErrorCustom(
				RESULT_ERROR_SYNTACTIC_ANALYSIS,
				String_fromFormat(
					"'%s' is not a valid character in floating point exponent",
					format_char(ch)
				),
				Array_fromArgs(
					1,
					Token_alloc(
						TOKEN_MARKER,
						TOKEN_CARET,
						WHITESPACE_NONE,
						TextRange_construct(
							lexer->currentChar,
							lexer->currentChar + 1,
							lexer->line,
							lexer->column
						),
						(union TokenValue){0}
					)
				)
			);
		}

		// Invalid character in integer literal
		if(!is_decimal_digit(ch)) {
			return LexerErrorCustom(
				RESULT_ERROR_SYNTACTIC_ANALYSIS,
				String_fromFormat(
					"'%s' is not a valid digit in integer literal",
					format_char(ch)
				),
				Array_fromArgs(
					1,
					Token_alloc(
						TOKEN_MARKER,
						TOKEN_CARET,
						WHITESPACE_NONE,
						TextRange_construct(
							lexer->currentChar,
							lexer->currentChar + 1,
							lexer->line,
							lexer->column
						),
						(union TokenValue){0}
					)
				)
			);
		}
	}
	// if(ch) lexer->currentChar--;

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar, lexer->line, lexer->column);

	// Copy the number
	String *numberStr = TextRange_toString(&range);
	assertf(numberStr != NULL);

	// Swift does not allow leading dots in floating point literals
	if(TextRange_peek(&range, 0) == '.') {
		return LexerError(
			String_fromFormat(
				"'%s' is not a valid floating point literal; it must be written '0%s'",
				numberStr->value, numberStr->value
			),
			Array_fromArgs(
				1,
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					WHITESPACE_NONE,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			)
		);
	}

	// Remove underscores
	String_replaceAll(numberStr, "_", "");

	fetch_next_whitespace(lexer);

	// Create a token
	Token *token = hasDot || hasExponent ?
		Token_alloc(TOKEN_LITERAL, TOKEN_FLOATING, __wh_bit, range, (union TokenValue){.floating = strtod(numberStr->value, NULL)}) :
		Token_alloc(TOKEN_LITERAL, TOKEN_INTEGER, __wh_bit, range, (union TokenValue){.integer = strtol(numberStr->value, NULL, 10)});
	assertf(token != NULL);

	// Free the string
	String_free(numberStr);

	// Add the token to the array
	Array_push(lexer->tokens, token);
	return LexerSuccess();
}


LexerResult __Lexer_tokenizePunctuatorsAndOperators(Lexer *lexer) {
	char *start = lexer->currentChar;

	enum TokenType type = TOKEN_INVALID;
	enum TokenKind kind = TOKEN_DEFAULT;

	#define match_as(str, _type, _kind) if(Lexer_match(lexer, str)) type = _type, kind = _kind

	// Swift operators (sorted by length)
	// match_as("<<=", TOKEN_OPERATOR, TOKEN_LEFT_SHIFT_ASSIGN);
	// else match_as(">>=", TOKEN_OPERATOR, TOKEN_RIGHT_SHIFT_ASSIGN);
	/*else*/ match_as("...", TOKEN_OPERATOR, TOKEN_RANGE);
	else match_as("..<", TOKEN_OPERATOR, TOKEN_HALF_OPEN_RANGE);
	// else match_as("===", TOKEN_OPERATOR, TOKEN_IDENTITY);
	// else match_as("!==", TOKEN_OPERATOR, TOKEN_NOT_IDENTITY);
	// else match_as("<<", TOKEN_OPERATOR, TOKEN_LEFT_SHIFT);
	// else match_as(">>", TOKEN_OPERATOR, TOKEN_RIGHT_SHIFT);
	else match_as("&&", TOKEN_OPERATOR, TOKEN_LOG_AND);
	else match_as("||", TOKEN_OPERATOR, TOKEN_LOG_OR);
	else match_as("??", TOKEN_OPERATOR, TOKEN_NULL_COALESCING);
	// else match_as("?.", TOKEN_OPERATOR, TOKEN_OPTIONAL_CHAINING);
	else match_as("==", TOKEN_OPERATOR, TOKEN_EQUALITY);
	else match_as("!=", TOKEN_OPERATOR, TOKEN_NOT_EQUALITY);
	else match_as(">=", TOKEN_OPERATOR, TOKEN_GREATER_EQUAL);
	else match_as("<=", TOKEN_OPERATOR, TOKEN_LESS_EQUAL);
	// else match_as("+=", TOKEN_OPERATOR, TOKEN_PLUS_ASSIGN);
	// else match_as("-=", TOKEN_OPERATOR, TOKEN_MINUS_ASSIGN);
	// else match_as("*=", TOKEN_OPERATOR, TOKEN_MULT_ASSIGN);
	// else match_as("/=", TOKEN_OPERATOR, TOKEN_DIV_ASSIGN);
	// else match_as("%%=", TOKEN_OPERATOR, TOKEN_MOD_ASSIGN);
	// else match_as("&=", TOKEN_OPERATOR, TOKEN_BIT_AND_ASSIGN);
	// else match_as("|=", TOKEN_OPERATOR, TOKEN_BIT_OR_ASSIGN);
	// else match_as("^=", TOKEN_OPERATOR, TOKEN_BIT_XOR_ASSIGN);
	else match_as("->", TOKEN_PUNCTUATOR, TOKEN_ARROW);
	else match_as("=", TOKEN_OPERATOR, TOKEN_EQUAL);
	else match_as(">", TOKEN_OPERATOR, TOKEN_GREATER);
	else match_as("<", TOKEN_OPERATOR, TOKEN_LESS);
	else match_as("+", TOKEN_OPERATOR, TOKEN_PLUS);
	else match_as("-", TOKEN_OPERATOR, TOKEN_MINUS);
	else match_as("*", TOKEN_OPERATOR, TOKEN_STAR);
	else match_as("/", TOKEN_OPERATOR, TOKEN_SLASH);
	// else match_as("%%", TOKEN_OPERATOR, TOKEN_PERCENT);
	// else match_as("&", TOKEN_OPERATOR, TOKEN_AMPERSAND);
	// else match_as("|", TOKEN_OPERATOR, TOKEN_PIPE);
	// else match_as("^", TOKEN_OPERATOR, TOKEN_CARET);
	// else match_as("~", TOKEN_OPERATOR, TOKEN_TILDE);

	else match_as("(", TOKEN_PUNCTUATOR, TOKEN_LEFT_PAREN);
	else match_as(")", TOKEN_PUNCTUATOR, TOKEN_RIGHT_PAREN);
	else match_as("{", TOKEN_PUNCTUATOR, TOKEN_LEFT_BRACE);
	else match_as("}", TOKEN_PUNCTUATOR, TOKEN_RIGHT_BRACE);
	// else match_as("[", TOKEN_PUNCTUATOR, TOKEN_LEFT_BRACKET);
	// else match_as("]", TOKEN_PUNCTUATOR, TOKEN_RIGHT_BRACKET);
	// else match_as(".", TOKEN_PUNCTUATOR, TOKEN_DOT);
	else match_as(",", TOKEN_PUNCTUATOR, TOKEN_COMMA);
	else match_as(":", TOKEN_PUNCTUATOR, TOKEN_COLON);
	else match_as(";", TOKEN_PUNCTUATOR, TOKEN_SEMICOLON);
	else match_as("=", TOKEN_PUNCTUATOR, TOKEN_EQUAL);
	// else match_as("@", TOKEN_PUNCTUATOR, TOKEN_AT);
	// else match_as("#", TOKEN_PUNCTUATOR, TOKEN_HASH);
	// else match_as("`", TOKEN_PUNCTUATOR, TOKEN_BACKTICK);
	else match_as("?", TOKEN_PUNCTUATOR, TOKEN_QUESTION);
	else match_as("!", TOKEN_PUNCTUATOR, TOKEN_EXCLAMATION);
	else return LexerNoMatch();

	#undef match_as

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar, lexer->line, lexer->column);

	fetch_next_whitespace(lexer);

	// Create a token
	Token *token = Token_alloc(type, kind, __wh_bit, range, (union TokenValue){0});

	// Add the token to the array
	Array_push(lexer->tokens, token);
	return LexerSuccess();
}


void Lexer_printTokens(Lexer *lexer) {
	if(!lexer) return;

	if(!lexer->tokens->size) {
		printf("No tokens to print\n");
		return;
	}

	for(size_t i = 0; i < lexer->tokens->size; i++) {
		Token_print((Token*)Array_get(lexer->tokens, i), 0, false);
	}
}
