#include <stdbool.h>
#include <string.h>

#include "compiler/lexer/Lexer.h"
#include "internal/String.h"
#include "internal/Array.h"
#include "inspector.h"
#include "assertf.h"

char __Lexer_resolveEscapedChar(char ch);
LexerResult __Lexer_tokenizeString(Lexer *tokenizer);
LexerResult __Lexer_tokenizeIdentifier(Lexer *tokenizer);
LexerResult __Lexer_tokenizeNumberLiteral(Lexer *tokenizer);
LexerResult __Lexer_tokenizeIntegerBasedLiteral(Lexer *tokenizer, int base);
LexerResult __Lexer_tokenizeBinaryLiteral(Lexer *tokenizer);
LexerResult __Lexer_tokenizeOctalLiteral(Lexer *tokenizer);
LexerResult __Lexer_tokenizeHexadecimalLiteral(Lexer *tokenizer);
LexerResult __Lexer_tokenizeDecimalLiteral(Lexer *tokenizer);

void Lexer_constructor(Lexer *tokenizer) {
	if(!tokenizer) return;

	tokenizer->source = NULL;
	tokenizer->sourceLength = 0;
	tokenizer->tokens = Array_alloc(2);
	tokenizer->currentChar = '\0';
	tokenizer->line = 1;
	tokenizer->column = 1;
}

void Lexer_destructor(Lexer *tokenizer) {
	if(!tokenizer) return;

	if(tokenizer->tokens) {
		for(size_t i = 0; i < tokenizer->tokens->size; i++) {
			Token_destructor((Token*)Array_get(tokenizer->tokens, i));
		}

		Array_destructor(tokenizer->tokens);
		tokenizer->tokens = NULL;
	}

	tokenizer->source = NULL;
	tokenizer->sourceLength = 0;
	tokenizer->currentChar = '\0';
	tokenizer->line = 0;
	tokenizer->column = 0;
}

char Lexer_peek(Lexer *tokenizer, size_t offset) {
	if(!tokenizer) return '\0';
	if(!tokenizer->currentChar) return '\0';
	if(tokenizer->currentChar + offset >= tokenizer->source + tokenizer->sourceLength) return '\0';

	return *(tokenizer->currentChar + offset);
}

size_t Lexer_compare(Lexer *tokenizer, char *str) {
	if(!tokenizer) return 0;
	if(!str) return 0;

	size_t length = strlen(str);

	if(strncmp(tokenizer->currentChar, str, length) != 0) return 0;

	return length;
}

size_t Lexer_match(Lexer *tokenizer, char *str) {
	if(!tokenizer) return 0;
	if(!str) return 0;

	size_t offset = Lexer_compare(tokenizer, str);
	if(offset == 0) return 0;

	tokenizer->currentChar += offset;
	return offset;
}

char Lexer_advance(Lexer *tokenizer) {
	if(!tokenizer) return '\0';
	if(!tokenizer->currentChar) return '\0';

	char ch = *++tokenizer->currentChar;
	// Prevent from advancing past the end of the string
	// if(ch == '\0') tokenizer->currentChar--;

	return ch;
}


#define is_binary_digit(ch) ((ch) == '0' || (ch) == '1')
#define is_octal_digit(ch) ((ch) >= '0' && (ch) <= '7')
#define is_decimal_digit(ch) ((ch) >= '0' && (ch) <= '9')
#define is_hexadecimal_digit(ch) (((ch) >= '0' && (ch) <= '9') || ((ch) >= 'a' && (ch) <= 'f') || ((ch) >= 'A' && (ch) <= 'F'))
#define is_base_digit(ch, base) (base == 2 ? is_binary_digit(ch) : base == 8 ? is_octal_digit(ch) : base == 16 ? is_hexadecimal_digit(ch) : is_decimal_digit(ch))
#define is_identifier_start(ch) ((ch) == '_' || ((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define is_identifier_part(ch) (is_identifier_start(ch) || is_decimal_digit(ch))


LexerResult Lexer_tokenize(Lexer *tokenizer, char *source) {
	assertf(tokenizer != NULL);

	// Reset the lexer
	Lexer_destructor(tokenizer);
	Lexer_constructor(tokenizer);

	tokenizer->line = 1;
	tokenizer->column = 1;
	tokenizer->source = source;
	tokenizer->sourceLength = strlen(source);
	tokenizer->currentChar = tokenizer->source;

	char ch;
	while((ch = *tokenizer->currentChar)) {

		// Update line and column counters
		if(ch == '\n') {
			tokenizer->line++;
			tokenizer->column = 1;
		} else {
			tokenizer->column++;
		}

		// Skip whitespace
		if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
			Lexer_advance(tokenizer);
			continue;
		}
		// Skip single-line comments
		else if(Lexer_match(tokenizer, "//")) {
			while(!Lexer_match(tokenizer, "\n")) {
				Lexer_advance(tokenizer);
			}

			continue;
		}
		// Skip multi-line comments
		else if(Lexer_match(tokenizer, "/*")) {
			while(!Lexer_match(tokenizer, "*/")) {
				Lexer_advance(tokenizer);
			}

			continue;
		}
		// Match strings
		else if(ch == '"' || ch == '\'') {
			LexerResult result = __Lexer_tokenizeString(tokenizer);
			if(!result.success) return result;

			continue;
		}
		// Match identifiers
		else if(is_identifier_start(ch)) {
			LexerResult result = __Lexer_tokenizeIdentifier(tokenizer);
			if(!result.success) return result;

			continue;
		}
		// Match numbers
		else if(
			is_decimal_digit(ch) ||
			(ch == '.' && is_decimal_digit(Lexer_peek(tokenizer, 1))) // Error state
		) {
			LexerResult result = __Lexer_tokenizeNumberLiteral(tokenizer);
			if(!result.success) return result;

			continue;
		}
		// Invalid character
		else {
			return LexerError(
				String_fromFormat("unexpected token '%s'", format_char(ch)),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						tokenizer->currentChar,
						tokenizer->currentChar + 1,
						tokenizer->line,
						tokenizer->column
					),
					(union TokenValue){0}
				)
			);
		}

		// Advance to the next character
		Lexer_advance(tokenizer);
	}

	// Create an EOF token
	TextRange range;
	TextRange_constructor(&range, tokenizer->currentChar, tokenizer->currentChar, tokenizer->line, tokenizer->column);

	Token *token = Token_alloc(TOKEN_EOF, TOKEN_DEFAULT, range, (union TokenValue){0});
	assertf(token != NULL);

	Array_push(tokenizer->tokens, token);

	return LexerSuccess();
}

char __Lexer_resolveEscapedChar(char ch) {
	switch(ch) {
		case '0': return '\0';
		case 'a': return '\a';
		case 'b': return '\b';
		case 'f': return '\f';
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case 'v': return '\v';
		case '\\': return '\\';
		case '\'': return '\'';
		case '"': return '"';
		case 'd': return 'd';
		case 'x': return 'x';
		case 'u': return 'u';
		default: return '\0';
	}
}

// TODO: Implement string parsing from specifiaction
LexerResult __Lexer_tokenizeString(Lexer *tokenizer) {
	char *start = tokenizer->currentChar;
	char ch = *tokenizer->currentChar;

	assertf(ch == '"' || ch == '\'', "Unexpected character '%s' (expected '\"' or \"'\" at the source stream head)", format_char(ch));

	char quote = ch;
	ch = Lexer_advance(tokenizer); // Consume the first character

	String *string = String_alloc("");

	// Match string
	while(ch != quote) {
		// Handle unterminated string literals
		if(ch == '\0') return LexerError(
				String_fromFormat("unterminated string literal"),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						start,
						start + 1,
						tokenizer->line,
						tokenizer->column
					),
					(union TokenValue){0}
				)
		);

		// This consumes two characters, so in case of `\<quote>`, both backslash
		// and the quote are consumed and therefore the loop will not terminate
		if(ch == '\\') {
			char escaped = Lexer_advance(tokenizer);
			// TODO: Add support for unicode and hex escapes according to the language specification
			ch = __Lexer_resolveEscapedChar(escaped);
		}

		String_appendChar(string, ch);

		ch = Lexer_advance(tokenizer);
	}
	// if(ch) tokenizer->currentChar--;

	// Consume the closing quote
	Lexer_advance(tokenizer);

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar + 1, tokenizer->line, tokenizer->column);

	// Create a token
	Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_STRING, range, (union TokenValue){.string = string});
	assertf(token != NULL);

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return LexerSuccess();
}

LexerResult __Lexer_tokenizeIdentifier(Lexer *tokenizer) {
	char *start = tokenizer->currentChar;
	char ch = *tokenizer->currentChar;

	assertf(is_identifier_start(ch), "Unexpected character '%s' (expected identifier start at the source stream head)", format_char(ch));

	// Match identifier
	while(is_identifier_part(ch)) {
		ch = Lexer_advance(tokenizer);
	}
	// if(ch) tokenizer->currentChar--; //?

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar, tokenizer->line, tokenizer->column);

	// Copy the identifier
	String *identifier = TextRange_toString(&range);
	assertf(identifier != NULL);

	// Set a token value
	union TokenValue value;
	value.identifier = identifier;

	// Resolve the identifier
	enum TokenKind kind = TOKEN_DEFAULT;

	// Look for keywords
	if(TextRange_compare(&range, "and")) kind = TOKEN_AND;
	else if(TextRange_compare(&range, "class")) kind = TOKEN_CLASS;
	else if(TextRange_compare(&range, "else")) kind = TOKEN_ELSE;
	else if(TextRange_compare(&range, "false")) kind = TOKEN_FALSE;
	else if(TextRange_compare(&range, "for")) kind = TOKEN_FOR;
	else if(TextRange_compare(&range, "fun")) kind = TOKEN_FUN;
	else if(TextRange_compare(&range, "if")) kind = TOKEN_IF;
	else if(TextRange_compare(&range, "null")) kind = TOKEN_NULL;
	else if(TextRange_compare(&range, "return")) kind = TOKEN_RETURN;
	else if(TextRange_compare(&range, "super")) kind = TOKEN_SUPER;
	else if(TextRange_compare(&range, "this")) kind = TOKEN_THIS;
	else if(TextRange_compare(&range, "true")) kind = TOKEN_TRUE;
	else if(TextRange_compare(&range, "var")) kind = TOKEN_VAR;
	else if(TextRange_compare(&range, "while")) kind = TOKEN_WHILE;

	// Create a token
	Token *token = Token_alloc(kind == TOKEN_DEFAULT ? TOKEN_IDENTIFIER : TOKEN_KEYWORD, kind, range, value);
	assertf(token != NULL);

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return LexerSuccess();
}

LexerResult __Lexer_tokenizeNumberLiteral(Lexer *tokenizer) {
	if(Lexer_compare(tokenizer, "0b")) return __Lexer_tokenizeBinaryLiteral(tokenizer);
	if(Lexer_compare(tokenizer, "0o")) return __Lexer_tokenizeOctalLiteral(tokenizer);
	if(Lexer_compare(tokenizer, "0x")) return __Lexer_tokenizeHexadecimalLiteral(tokenizer);

	return __Lexer_tokenizeDecimalLiteral(tokenizer);
}

LexerResult __Lexer_tokenizeIntegerBasedLiteral(Lexer *tokenizer, int base) {
	char *prefix =
	base == 2 ? "0b" :
		base == 8 ? "0o" :
			base == 16 ? "0x" :
				NULL;

	assertf(prefix != NULL, "Invalid base '%d'; Allowed bases are 2, 8 and 16", base);

	char *start = tokenizer->currentChar;

	// Check and consume the prefix
	assertf(Lexer_match(tokenizer, prefix) != 0, "Expected '%s' at the source stream head", prefix);

	char ch = *tokenizer->currentChar;
	if(!is_base_digit(ch, base)) return LexerError(
			String_fromFormat(
				"'%s' is not a valid %s in integer literal", format_char(ch),
				base == 2 ? "binary digit (0 or 1)" :
					base == 8 ? "octal digit (0-7)" :
						base == 16 ? "hexadecimal digit (0-9, a-f)" : "digit"
			),
			Token_alloc(
				TOKEN_MARKER,
				TOKEN_CARET,
				TextRange_construct(
					tokenizer->currentChar,
					tokenizer->currentChar + 1,
					tokenizer->line,
					tokenizer->column
				),
				(union TokenValue){0}
			)
	);

	// Match number
	while(is_base_digit(*tokenizer->currentChar, base) || ch == '_') {
		ch = Lexer_advance(tokenizer);
	}
	// if(ch) tokenizer->currentChar--; //?

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar, tokenizer->line, tokenizer->column);

	// Copy the number
	String *numberStr = TextRange_toString(&range);
	assertf(numberStr != NULL);

	// Parse the number and free the string
	long number = strtol(numberStr->value + 2, NULL, base);
	String_free(numberStr);

	// Create a token
	Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_INTEGER, range, (union TokenValue){.integer = number});

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return LexerSuccess();
}

LexerResult __Lexer_tokenizeBinaryLiteral(Lexer *tokenizer) {
	return __Lexer_tokenizeIntegerBasedLiteral(tokenizer, 2);
}

LexerResult __Lexer_tokenizeOctalLiteral(Lexer *tokenizer) {
	return __Lexer_tokenizeIntegerBasedLiteral(tokenizer, 8);
}

LexerResult __Lexer_tokenizeHexadecimalLiteral(Lexer *tokenizer) {
	// TODO: Currently not supporting floating point hexadecimal literals
	return __Lexer_tokenizeIntegerBasedLiteral(tokenizer, 16);
}

LexerResult __Lexer_tokenizeDecimalLiteral(Lexer *tokenizer) {
	char *start = tokenizer->currentChar;
	char ch = *tokenizer->currentChar;

	assertf(is_decimal_digit(ch) || ch == '.', "Unexpected character '%s' (expected decimal digit at the source stream head)", format_char(ch));

	// Match number
	// TODO: Add support for other bases, exponents and checks like '.5' and '075' according to the language specification; Update: none of these are supported
	// Leading zeros are allowed
	bool hasDot = false;
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
	while(is_decimal_digit(ch) || ch == '.') {
		if(ch == '.') {
			// if(hasDot) return Result(RESULT_ERROR_STATIC_LEXICAL_ANALYSIS, "Unexpected character '.' in number");
			if(hasDot) break;       // Accessor (ex. 10.123.toFixed())
			hasDot = true;
		}

		ch = Lexer_advance(tokenizer);
	}
	// if(ch) tokenizer->currentChar--;

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar, tokenizer->line, tokenizer->column);

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
			Token_alloc(
				TOKEN_MARKER,
				TOKEN_CARET,
				TextRange_construct(
					tokenizer->currentChar,
					tokenizer->currentChar + 1,
					tokenizer->line,
					tokenizer->column
				),
				(union TokenValue){0}
			)
		);
	}

	// Create a token
	Token *token = hasDot ?
		Token_alloc(TOKEN_LITERAL, TOKEN_FLOATING, range, (union TokenValue){.floating = strtod(numberStr->value, NULL)}) :
		Token_alloc(TOKEN_LITERAL, TOKEN_INTEGER, range, (union TokenValue){.integer = strtol(numberStr->value, NULL, 10)});
	assertf(token != NULL);

	// Free the string
	String_free(numberStr);

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return LexerSuccess();
}


// LexerResult __Lexer_tokenizeSymbols(Lexer *tokenizer) {
// 	char *start = tokenizer->currentChar;
// 	char ch = *start;

// 	TokenType type = TOKEN_UNKNOWN;

// 	if(Lexer_match(tokenizer, "//")) type = 10;

// 	// Create a TextRange view
// 	TextRange range;
// 	TextRange_constructor(&range, start, tokenizer->currentChar, tokenizer->line, tokenizer->column);

void Lexer_printTokens(Lexer *tokenizer) {
	if(!tokenizer) return;

	for(size_t i = 0; i < tokenizer->tokens->size; i++) {
		Token_print((Token*)Array_get(tokenizer->tokens, i), 0, false);
	}
}
