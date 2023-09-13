#include <stdbool.h>
#include <string.h>

#include "compiler/lexer/Lexer.h"
#include "internal/String.h"

char __Lexer_resolveEscapedChar(char ch);
Result __Lexer_tokenizeString(Lexer *tokenizer);
Result __Lexer_tokenizeIdentifier(Lexer *tokenizer);
Result __Lexer_tokenizeNumber(Lexer *tokenizer);

void Lexer_constructor(Lexer *tokenizer) {
	if(!tokenizer) return;

	tokenizer->source = NULL;
	tokenizer->tokens = Array_alloc(2);
	tokenizer->currentChar = '\0';
	tokenizer->line = 1;
	tokenizer->column = 1;
}

void Lexer_destructor(Lexer *tokenizer) {
	if(!tokenizer) return;

	if(tokenizer->tokens) {
		for(size_t i = 0; i < tokenizer->tokens->size; i++) {
			Token_destructor(Array_get(tokenizer->tokens, i));
		}

		Array_destructor(tokenizer->tokens);
		tokenizer->tokens = NULL;
	}

	tokenizer->source = NULL;
	tokenizer->currentChar = '\0';
	tokenizer->line = 0;
	tokenizer->column = 0;
}

char Lexer_peek(Lexer *tokenizer, size_t offset) {
	if(!tokenizer) return '\0';

	return *(tokenizer->currentChar + offset);
}

size_t Lexer_compare(Lexer *tokenizer, char *str) {
	if(!tokenizer) return 0;
	if(!str) return 0;

	char *ptr = strstr(tokenizer->currentChar, str);
	if(!ptr) return 0;

	return ptr - tokenizer->currentChar;
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

	char ch = *tokenizer->currentChar++;
	// Prevent from advancing past the end of the string
	if(ch == '\0') tokenizer->currentChar--;

	return ch;
}

Result Lexer_tokenize(Lexer *tokenizer, char *source) {
	if(!tokenizer) return Result(RESULT_ASSERTION);

	tokenizer->line = 1;
	tokenizer->column = 1;
	tokenizer->source = source;
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
			Result result = __Lexer_tokenizeString(tokenizer);
			if(!result.success) return result;
		}
		// Match identifiers
		else if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
			Result result = __Lexer_tokenizeIdentifier(tokenizer);
			if(!result.success) return result;
		}
		// Match numbers
		else if((ch >= '0' && ch <= '9') || ch == '.') {
			Result result = __Lexer_tokenizeNumber(tokenizer);
			if(!result.success) return result;
		}
		// Invalid character
		else {
			return Result(RESULT_ERROR_STATIC_LEXICAL_ANALYSIS, "Unexpected token");
		}

		// Advance to the next character
		Lexer_advance(tokenizer);
	}

	// Create an EOF token
	TextRange range;
	TextRange_constructor(&range, tokenizer->currentChar, tokenizer->currentChar, tokenizer->line, tokenizer->column);

	Token *token = Token_alloc(TOKEN_EOF, (union TokenValue){.number = -1}, range);
	if(!token) return Result(RESULT_ERROR_INTERNAL);

	Array_push(tokenizer->tokens, token);

	return Result(RESULT_SUCCESS);
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

Result __Lexer_tokenizeString(Lexer *tokenizer) {
	char *start = tokenizer->currentChar;
	char ch = *tokenizer->currentChar;

	if(ch != '"' && ch != '\'') return Result(RESULT_ERROR_STATIC_LEXICAL_ANALYSIS, "Unexpected character (expected '\"' or \"'\")");
	Lexer_advance(tokenizer); // Consume the opening quote
	ch = Lexer_advance(tokenizer); // Consume the first character

	String *string = String_alloc(NULL);

	// Match string
	while(ch != '"' && ch != '\'') {
		if(ch == '\0') return Result(RESULT_ERROR_STATIC_LEXICAL_ANALYSIS, "Unexpected end of input (unterminated string)");
		if(ch == '\\') {
			char escaped = Lexer_advance(tokenizer);
			// TODO: Add support for unicode and hex escapes according to the language specification
			ch = __Lexer_resolveEscapedChar(escaped);
		}

		String_appendChar(string, ch);

		ch = Lexer_advance(tokenizer);
	}
	tokenizer->currentChar--;

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar + 1, tokenizer->line, tokenizer->column);

	// Create a token
	Token *token = Token_alloc(TOKEN_STRING, (union TokenValue){.string = string}, range);
	if(!token) return Result(RESULT_ERROR_INTERNAL);

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return Result(RESULT_SUCCESS);
}

Result __Lexer_tokenizeIdentifier(Lexer *tokenizer) {
	char ch = *tokenizer->currentChar;

	// Match identifier
	char *start = tokenizer->currentChar;
	while((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9')) {
		ch = Lexer_advance(tokenizer);
	}
	tokenizer->currentChar--;

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar, tokenizer->line, tokenizer->column);

	// Copy the identifier
	String *identifier = TextRange_getString(&range);
	if(!identifier && range.length > 0) return Result(RESULT_ERROR_INTERNAL);

	// Set a token value
	union TokenValue value;
	value.identifier = identifier->value;

	// Resolve the identifier
	enum TokenType type = TOKEN_IDENTIFIER;

	// Look for keywords
	if(TextRange_compare(&range, "and")) type = TOKEN_AND;
	else if(TextRange_compare(&range, "class")) type = TOKEN_CLASS;
	else if(TextRange_compare(&range, "else")) type = TOKEN_ELSE;
	else if(TextRange_compare(&range, "false")) type = TOKEN_FALSE;
	else if(TextRange_compare(&range, "for")) type = TOKEN_FOR;
	else if(TextRange_compare(&range, "fun")) type = TOKEN_FUN;
	else if(TextRange_compare(&range, "if")) type = TOKEN_IF;
	else if(TextRange_compare(&range, "null")) type = TOKEN_NULL;
	else if(TextRange_compare(&range, "return")) type = TOKEN_RETURN;
	else if(TextRange_compare(&range, "super")) type = TOKEN_SUPER;
	else if(TextRange_compare(&range, "this")) type = TOKEN_THIS;
	else if(TextRange_compare(&range, "true")) type = TOKEN_TRUE;
	else if(TextRange_compare(&range, "var")) type = TOKEN_VAR;
	else if(TextRange_compare(&range, "while")) type = TOKEN_WHILE;

	// Create a token
	Token *token = Token_alloc(type, value, range);
	if(!token) return Result(RESULT_ERROR_INTERNAL);

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return Result(RESULT_SUCCESS);
}

Result __Lexer_tokenizeNumber(Lexer *tokenizer) {
	char ch = *tokenizer->currentChar;

	// Match number
	// TODO: Add support for other bases, exponents and checks like '.5' and '075' according to the language specification
	char *start = tokenizer->currentChar;
	bool hasDot = false;
	while((ch >= '0' && ch <= '9') || ch == '.') {
		if(ch == '.') {
			if(hasDot) return Result(RESULT_ERROR_STATIC_LEXICAL_ANALYSIS, "Unexpected character '.' in number");
			hasDot = true;
		}

		ch = Lexer_advance(tokenizer);
	}
	tokenizer->currentChar--;

	// Create a TextRange view
	TextRange range;
	TextRange_constructor(&range, start, tokenizer->currentChar, tokenizer->line, tokenizer->column);

	// Copy the number
	String *numberStr = TextRange_getString(&range);
	if(!numberStr && range.length > 0) return Result(RESULT_ERROR_INTERNAL);

	// Parse the number and free the string
	double number = strtod(numberStr->value, NULL);
	String_free(numberStr);

	// Create a token
	Token *token = Token_alloc(TOKEN_NUMBER, (union TokenValue){.number = number}, range);
	if(!token) return Result(RESULT_ERROR_INTERNAL);

	// Add the token to the array
	Array_push(tokenizer->tokens, token);
	return Result(RESULT_SUCCESS);
}
