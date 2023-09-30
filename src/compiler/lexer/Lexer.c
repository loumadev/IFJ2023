#include <stdbool.h>
#include <string.h>

#include "compiler/lexer/Lexer.h"
#include "internal/String.h"
#include "internal/Array.h"
#include "inspector.h"
#include "assertf.h"

char __Lexer_resolveEscapedChar(char ch);

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
	lexer->currentChar = NULL;
	lexer->line = 1;
	lexer->column = 1;
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
	lexer->line = 0;
	lexer->column = 0;
}

char Lexer_peek(Lexer *lexer, size_t offset) {
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

#define is_alpha(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define is_binary_digit(ch) ((ch) == '0' || (ch) == '1')
#define is_octal_digit(ch) ((ch) >= '0' && (ch) <= '7')
#define is_decimal_digit(ch) ((ch) >= '0' && (ch) <= '9')
#define is_hexadecimal_digit(ch) (((ch) >= '0' && (ch) <= '9') || ((ch) >= 'a' && (ch) <= 'f') || ((ch) >= 'A' && (ch) <= 'F'))
#define is_base_digit(ch, base) (base == 2 ? is_binary_digit(ch) : base == 8 ? is_octal_digit(ch) : base == 16 ? is_hexadecimal_digit(ch) : is_decimal_digit(ch))
#define is_identifier_start(ch) ((ch) == '_' || is_alpha(ch))
#define is_identifier_part(ch) (is_identifier_start(ch) || is_decimal_digit(ch))


LexerResult Lexer_tokenize(Lexer *lexer, char *source) {
	assertf(lexer != NULL);

	// Reset the lexer
	Lexer_destructor(lexer);
	Lexer_constructor(lexer);

	lexer->line = 1;
	lexer->column = 1;
	lexer->source = source;
	lexer->sourceLength = strlen(source);
	lexer->currentChar = lexer->source;

	char ch;
	while((ch = *lexer->currentChar)) {

		// Update line and column counters
		if(ch == '\n') {
			lexer->line++;
			lexer->column = 1;
		} else {
			lexer->column++;
		}

		// Skip whitespace
		if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
			Lexer_advance(lexer);
			continue;
		}
		// Skip single-line comments
		else if(Lexer_match(lexer, "//")) {
			while(!Lexer_isAtEnd(lexer) && !Lexer_match(lexer, "\n")) {
				Lexer_advance(lexer);
			}

			continue;
		}
		// Skip multi-line comments
		else if(Lexer_match(lexer, "/*")) {
			size_t depth = 1;

			while(!Lexer_isAtEnd(lexer)) {
				if(Lexer_match(lexer, "/*")) {
					depth++;
				} else if(Lexer_match(lexer, "*/")) {
					depth--;

					if(depth == 0) break;
				}

				Lexer_advance(lexer);
			}

			if(depth == 0) continue;

			// There are still comments left
			return LexerError(
				String_fromFormat("unterminated '/*' comment"),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			);
		} else if(Lexer_match(lexer, "*/")) {
			// There are no comments to close
			return LexerError(
				String_fromFormat("unexpected end of block comment"),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			);
		}
		// Match strings
		else if(ch == '"' || ch == '\'') {
			LexerResult result = __Lexer_tokenizeString(lexer);
			if(!result.success) return result;

			continue;
		}
		// Match identifiers
		else if(is_identifier_start(ch)) {
			LexerResult result = __Lexer_tokenizeIdentifier(lexer);
			if(!result.success) return result;

			continue;
		}
		// Match numbers
		else if(
			is_decimal_digit(ch) ||
			(ch == '.' && is_decimal_digit(Lexer_peek(lexer, 1))) // Error state
		) {
			LexerResult result = __Lexer_tokenizeNumberLiteral(lexer);
			if(!result.success) return result;

			continue;
		}
		// Something other
		else {
			// Try to match punctuators and operators
			LexerResult result = __Lexer_tokenizePunctuatorsAndOperators(lexer);
			if(result.success) continue;

			// Invalid character
			return LexerError(
				String_fromFormat("unexpected token '%s'", format_char(ch)),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			);
		}

		// Advance to the next character
		Lexer_advance(lexer);
	}

	// Create an EOF token
	TextRange range;
	TextRange_constructor(&range, lexer->currentChar, lexer->currentChar, lexer->line, lexer->column);

	Token *token = Token_alloc(TOKEN_EOF, TOKEN_DEFAULT, range, (union TokenValue){0});
	assertf(token != NULL);

	Array_push(lexer->tokens, token);

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
		if(ch == '\0') return LexerError(
				String_fromFormat("unterminated string literal"),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						start,
						start + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
		);

		// This consumes two characters, so in case of `\<quote>`, both backslash
		// and the quote are consumed and therefore the loop will not terminate
		if(ch == '\\') {
			char escaped = Lexer_advance(lexer);
			// TODO: Add support for unicode and hex escapes according to the language specification
			ch = __Lexer_resolveEscapedChar(escaped);
		}

		String_appendChar(string, ch);

		ch = Lexer_advance(lexer);
	}
	// if(ch) lexer->currentChar--;

	// Consume the closing quote
	Lexer_advance(lexer);

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar + 1, lexer->line, lexer->column);

	// Create a token
	Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_STRING, range, (union TokenValue){.string = string});
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

	// Copy the identifier
	String *identifier = TextRange_toString(&range);
	assertf(identifier != NULL);

	// Set a token value
	union TokenValue value;
	value.identifier = identifier;

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

	// Create a token
	Token *token = Token_alloc(type == TOKEN_INVALID ? kind == TOKEN_DEFAULT ? TOKEN_IDENTIFIER : TOKEN_KEYWORD : type, kind, range, value);
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
			Token_alloc(
				TOKEN_MARKER,
				TOKEN_CARET,
				TextRange_construct(
					lexer->currentChar,
					lexer->currentChar + 1,
					lexer->line,
					lexer->column
				),
				(union TokenValue){0}
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

	// Create a token
	Token *token = Token_alloc(TOKEN_LITERAL, TOKEN_INTEGER, range, (union TokenValue){.integer = number});

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
			if(hasDot) break;       // Accessor (ex. 10.123.toFixed())
			if(is_identifier_start(Lexer_peek(lexer, 1))) break;     // Accessor (ex. 10.toFixed())
			if(!is_decimal_digit(Lexer_peek(lexer, 1))) return LexerError(
					String_fromFormat("expected member name following '.'"),
					Token_alloc(
						TOKEN_MARKER,
						TOKEN_CARET,
						TextRange_construct(
							lexer->currentChar,
							lexer->currentChar + 1,
							lexer->line,
							lexer->column
						),
						(union TokenValue){0}
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
			if(!is_decimal_digit(ch)) return LexerError(
					String_fromFormat("expected a digit in floating point exponent"),
					Token_alloc(
						TOKEN_MARKER,
						TOKEN_CARET,
						TextRange_construct(
							lexer->currentChar,
							lexer->currentChar + 1,
							lexer->line,
							lexer->column
						),
						(union TokenValue){0}
					)
			);
		}

		// Handle invalid characters
		if(!is_alpha(ch)) continue;

		// Invalid character in exponent
		if(!is_decimal_digit(ch) && hasExponent) {
			return LexerError(
				String_fromFormat(
					"'%s' is not a valid character in floating point exponent",
					format_char(ch)
				),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
				)
			);
		}

		// Invalid character in integer literal
		if(!is_decimal_digit(ch)) {
			return LexerError(
				String_fromFormat(
					"'%s' is not a valid digit in integer literal",
					format_char(ch)
				),
				Token_alloc(
					TOKEN_MARKER,
					TOKEN_CARET,
					TextRange_construct(
						lexer->currentChar,
						lexer->currentChar + 1,
						lexer->line,
						lexer->column
					),
					(union TokenValue){0}
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
			Token_alloc(
				TOKEN_MARKER,
				TOKEN_CARET,
				TextRange_construct(
					lexer->currentChar,
					lexer->currentChar + 1,
					lexer->line,
					lexer->column
				),
				(union TokenValue){0}
			)
		);
	}

	// Remove underscores
	String_replaceAll(numberStr, "_", "");

	// Create a token
	Token *token = hasDot || hasExponent ?
		Token_alloc(TOKEN_LITERAL, TOKEN_FLOATING, range, (union TokenValue){.floating = strtod(numberStr->value, NULL)}) :
		Token_alloc(TOKEN_LITERAL, TOKEN_INTEGER, range, (union TokenValue){.integer = strtol(numberStr->value, NULL, 10)});
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
	match_as("<<=", TOKEN_OPERATOR, TOKEN_LEFT_SHIFT_ASSIGN);
	else match_as(">>=", TOKEN_OPERATOR, TOKEN_RIGHT_SHIFT_ASSIGN);
	else match_as("...", TOKEN_OPERATOR, TOKEN_RANGE);
	else match_as("..<", TOKEN_OPERATOR, TOKEN_HALF_OPEN_RANGE);
	else match_as("===", TOKEN_OPERATOR, TOKEN_IDENTITY);
	else match_as("!==", TOKEN_OPERATOR, TOKEN_NOT_IDENTITY);
	else match_as("<<", TOKEN_OPERATOR, TOKEN_LEFT_SHIFT);
	else match_as(">>", TOKEN_OPERATOR, TOKEN_RIGHT_SHIFT);
	else match_as("&&", TOKEN_OPERATOR, TOKEN_LOG_AND);
	else match_as("||", TOKEN_OPERATOR, TOKEN_LOG_OR);
	else match_as("??", TOKEN_OPERATOR, TOKEN_NULL_COALESCING);
	else match_as("?.", TOKEN_OPERATOR, TOKEN_OPTIONAL_CHAINING);
	else match_as("==", TOKEN_OPERATOR, TOKEN_EQUALITY);
	else match_as("!=", TOKEN_OPERATOR, TOKEN_NOT_EQUALITY);
	else match_as("+=", TOKEN_OPERATOR, TOKEN_PLUS_ASSIGN);
	else match_as("-=", TOKEN_OPERATOR, TOKEN_MINUS_ASSIGN);
	else match_as("*=", TOKEN_OPERATOR, TOKEN_MULT_ASSIGN);
	else match_as("/=", TOKEN_OPERATOR, TOKEN_DIV_ASSIGN);
	else match_as("%%=", TOKEN_OPERATOR, TOKEN_MOD_ASSIGN);
	else match_as("&=", TOKEN_OPERATOR, TOKEN_BIT_AND_ASSIGN);
	else match_as("|=", TOKEN_OPERATOR, TOKEN_BIT_OR_ASSIGN);
	else match_as("^=", TOKEN_OPERATOR, TOKEN_BIT_XOR_ASSIGN);
	else match_as("->", TOKEN_PUNCTUATOR, TOKEN_ARROW);
	else match_as("+", TOKEN_OPERATOR, TOKEN_PLUS);
	else match_as("-", TOKEN_OPERATOR, TOKEN_MINUS);
	else match_as("*", TOKEN_OPERATOR, TOKEN_STAR);
	else match_as("/", TOKEN_OPERATOR, TOKEN_SLASH);
	else match_as("%%", TOKEN_OPERATOR, TOKEN_PERCENT);
	else match_as("&", TOKEN_OPERATOR, TOKEN_AMPERSAND);
	else match_as("|", TOKEN_OPERATOR, TOKEN_PIPE);
	else match_as("^", TOKEN_OPERATOR, TOKEN_CARET);
	else match_as("~", TOKEN_OPERATOR, TOKEN_TILDE);

	else match_as("{", TOKEN_PUNCTUATOR, TOKEN_LEFT_BRACE);
	else match_as("}", TOKEN_PUNCTUATOR, TOKEN_RIGHT_BRACE);
	else match_as("[", TOKEN_PUNCTUATOR, TOKEN_LEFT_BRACKET);
	else match_as("]", TOKEN_PUNCTUATOR, TOKEN_RIGHT_BRACKET);
	else match_as(".", TOKEN_PUNCTUATOR, TOKEN_DOT);
	else match_as(",", TOKEN_PUNCTUATOR, TOKEN_COMMA);
	else match_as(":", TOKEN_PUNCTUATOR, TOKEN_COLON);
	else match_as(";", TOKEN_PUNCTUATOR, TOKEN_SEMICOLON);
	else match_as("=", TOKEN_PUNCTUATOR, TOKEN_EQUAL);
	else match_as("@", TOKEN_PUNCTUATOR, TOKEN_AT);
	else match_as("#", TOKEN_PUNCTUATOR, TOKEN_HASH);
	else match_as("&", TOKEN_PUNCTUATOR, TOKEN_AMPERSAND);
	else match_as("`", TOKEN_PUNCTUATOR, TOKEN_BACKTICK);
	else match_as("?", TOKEN_PUNCTUATOR, TOKEN_QUESTION);
	else match_as("!", TOKEN_PUNCTUATOR, TOKEN_EXCLAMATION);

	#undef match_as

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, lexer->currentChar, lexer->line, lexer->column);

	// Create a token
	Token *token = Token_alloc(type, kind, range, (union TokenValue){0});

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
