#include "compiler/lexer/Lexer.h"
#include "unit.h"
#include <stdio.h>

#include "../parser/parser_assertions.h"

#define TEST_PRIORITY 90

DESCRIBE(invalid_tokens, "Invalid tokens") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST_BEGIN("Invalid character") {
		result = Lexer_tokenize(&lexer, "ľ");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "ý");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "~");
		EXPECT_FALSE(result.success);
	} TEST_END();
}

DESCRIBE(comment_stripping, "Comments stripping") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST_BEGIN("Single line comment") {
		result = Lexer_tokenize(&lexer, "A // comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		result = Lexer_tokenize(&lexer, "A // comment\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		result = Lexer_tokenize(&lexer, "A // comment\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A // comment\nB // comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A // comment\n// comment\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A //\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A //\n//\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);
	} TEST_END();

	TEST_BEGIN("Multiline comment") {
		result = Lexer_tokenize(&lexer, "A /* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		result = Lexer_tokenize(&lexer, "A /* comment */\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		result = Lexer_tokenize(&lexer, "A /* comment */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A /* comment */\nB /* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A /* comment */\n/* comment */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A/**/B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A/* */B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A/*\n*/B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A\n/*\nX\n*/\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);
	} TEST_END();

	TEST_BEGIN("Nested multiline comment") {
		result = Lexer_tokenize(&lexer, "A /* comment /* nested1 */ */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		result = Lexer_tokenize(&lexer, "A /* comment /* nested1 */ */\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		result = Lexer_tokenize(&lexer, "A /* comment /* nested1 */ */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A /* comment\n /* nested1 */ */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A /* comment\n /* nested1\n */ */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "A /* comment\n /* nested1\n /* nested2\n /* nested3\n /* nested4\n */ \nX */ X\n */ \nX\n */ X */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);
	} TEST_END();

	TEST("Invalid use of multiline comment", {
		result = Lexer_tokenize(&lexer, "A /* comment");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A comment */ X");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A /* comment\nB");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A /* comment\nB /* nested1");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A /* comment\nB /* nested1\nC");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A /* comment\nB /* nested1 */ \nC");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A /* comment\nB */ nested1 */ \nC");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "A /* comment\nB /* nested1 /* nested1 /* nested1 */ X */ \nC");
		EXPECT_FALSE(result.success);
	});
}

DESCRIBE(whitespace_resolution, "Whitespace resolution") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST_BEGIN("Simple whitespace") {
		result = Lexer_tokenize(&lexer, "A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == /*WHITESPACE_LEFT_LIMIT*/ WHITESPACE_NONE);


		result = Lexer_tokenize(&lexer, " A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_SPACE);


		result = Lexer_tokenize(&lexer, "A ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (/*WHITESPACE_LEFT_LIMIT*/ WHITESPACE_NONE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, " A ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "\nA");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_NEWLINE);


		result = Lexer_tokenize(&lexer, "A\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (/*WHITESPACE_LEFT_LIMIT*/ WHITESPACE_NONE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "\nA\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, " A\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "\nA ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));
	} TEST_END();

	TEST_BEGIN("Simple comment") {
		result = Lexer_tokenize(&lexer, "A// comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_NEWLINE);


		result = Lexer_tokenize(&lexer, "A // comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_NEWLINE);


		result = Lexer_tokenize(&lexer, "A\n// comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_NEWLINE);


		result = Lexer_tokenize(&lexer, "// comment\nA");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_NEWLINE);


		result = Lexer_tokenize(&lexer, "// comment\n A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_NEWLINE);


		result = Lexer_tokenize(&lexer, "A/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_SPACE);


		result = Lexer_tokenize(&lexer, "A /* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_SPACE);


		result = Lexer_tokenize(&lexer, "A/* comment \n comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_NEWLINE);


		result = Lexer_tokenize(&lexer, "A /* comment \n comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 00);
		EXPECT_TRUE(token->whitespace == WHITESPACE_RIGHT_NEWLINE);


		result = Lexer_tokenize(&lexer, "/* comment */A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_SPACE);


		result = Lexer_tokenize(&lexer, "/* comment */ A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_SPACE);


		result = Lexer_tokenize(&lexer, "/* comment \n comment */A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_NEWLINE);


		result = Lexer_tokenize(&lexer, "/* comment \n comment */ A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == WHITESPACE_LEFT_NEWLINE);
	} TEST_END();


	TEST_BEGIN("Multiple whitespace") {
		result = Lexer_tokenize(&lexer, "  A  ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "\n\nA\n\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));
	} TEST_END();


	TEST_BEGIN("Multiple comments") {
		result = Lexer_tokenize(&lexer, "// comment\nA// comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "// comment\n A// comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "// comment\nA // comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "// comment\n A // comment");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "/* comment */A/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "/* comment */A/* comment \n comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "/* comment \n comment */A/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "/* comment \n comment */A/* comment \n comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "/* comment */\nA/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "/* comment */A \n/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));
	} TEST_END();

	TEST_BEGIN("Mixed whitespace") {
		result = Lexer_tokenize(&lexer, " \nA  ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "\n A  ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));

		result = Lexer_tokenize(&lexer, "  A \n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, " \nA\n ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));
	} TEST_END();

	TEST_BEGIN("Mixed comments") {
		result = Lexer_tokenize(&lexer, "// comment\nA/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "/* comment */A// comment\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "/* comment */\nA// comment\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_RIGHT_NEWLINE | WHITESPACE_LEFT_NEWLINE));


		result = Lexer_tokenize(&lexer, "// comment\nA\n/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_RIGHT_NEWLINE | WHITESPACE_LEFT_NEWLINE));


		result = Lexer_tokenize(&lexer, "// comment\n A /* comment \n comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_RIGHT_NEWLINE | WHITESPACE_LEFT_NEWLINE));
	} TEST_END();

	TEST_BEGIN("Whitespace in context") {
		result = Lexer_tokenize(&lexer, "// comment\nA/* comment */B\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "// comment\nA/* comment */\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_NONE));


		result = Lexer_tokenize(&lexer, "// comment\nA/* comment */\nB\n");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_NEWLINE));


		result = Lexer_tokenize(&lexer, "/* comment */A/* comment */B/* comment */");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));


		result = Lexer_tokenize(&lexer, "A = B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));
		EXPECT_TRUE(token->whitespace & WHITESPACE_LEFT);
		EXPECT_TRUE(token->whitespace & WHITESPACE_RIGHT);
		EXPECT_TRUE(whitespace_both(token->whitespace));


		result = Lexer_tokenize(&lexer, "A =B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_NONE));
		EXPECT_TRUE(token->whitespace & WHITESPACE_LEFT);
		EXPECT_FALSE(token->whitespace & WHITESPACE_RIGHT);
		EXPECT_FALSE(whitespace_both(token->whitespace));


		result = Lexer_tokenize(&lexer, "A= B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_NONE | WHITESPACE_RIGHT_SPACE));
		EXPECT_FALSE(token->whitespace & WHITESPACE_LEFT);
		EXPECT_TRUE(token->whitespace & WHITESPACE_RIGHT);
		EXPECT_FALSE(whitespace_both(token->whitespace));


		result = Lexer_tokenize(&lexer, "A\n= B");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_NEWLINE | WHITESPACE_RIGHT_SPACE));
		EXPECT_TRUE(token->whitespace & WHITESPACE_LEFT);
		EXPECT_TRUE(token->whitespace & WHITESPACE_RIGHT);
		EXPECT_TRUE(whitespace_both(token->whitespace));


		result = Lexer_tokenize(&lexer, "A/**/=\nB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_NEWLINE));
		EXPECT_TRUE(token->whitespace & WHITESPACE_LEFT);
		EXPECT_TRUE(token->whitespace & WHITESPACE_RIGHT);
		EXPECT_TRUE(whitespace_both(token->whitespace));
	} TEST_END();
}

DESCRIBE(number_tokenization, "Number literals tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Single integer in context", {
		result = Lexer_tokenize(&lexer, "7");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);


		result = Lexer_tokenize(&lexer, " 7 ");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 7);


		result = Lexer_tokenize(&lexer, "7 a");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 7);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "a"));
	})


	TEST("Multidigit integer", {
		result = Lexer_tokenize(&lexer, "256");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 256);
	})


	TEST("Singledigit float", {
		result = Lexer_tokenize(&lexer, "0.5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 0.5);
	})


	TEST("Multidigit float", {
		result = Lexer_tokenize(&lexer, "123.14159265359");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 123.14159265359);
	})


	TEST_BEGIN("Leading dot in float literal") {
		result = Lexer_tokenize(&lexer, ".5");
		EXPECT_FALSE(result.success);
		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);
	} TEST_END();

	TEST("Invalid digits in float literal", {
		result = Lexer_tokenize(&lexer, "y.0");
		EXPECT_FALSE(result.success);
		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);

		result = Lexer_tokenize(&lexer, "1.0y");
		EXPECT_TRUE(result.success);

		result = Lexer_tokenize(&lexer, "1y.0");
		EXPECT_FALSE(result.success);
		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);

		result = Lexer_tokenize(&lexer, "10.20y");
		EXPECT_TRUE(result.success);

		result = Lexer_tokenize(&lexer, "10y.20");
		EXPECT_FALSE(result.success);
		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);

		result = Lexer_tokenize(&lexer, "10y.20y6");
		EXPECT_FALSE(result.success);
		EXPECT_TRUE(result.type == RESULT_ERROR_LEXICAL_ANALYSIS);
	})


	// TEST("Invalid digits in based integer literal", {
	// 	result = Lexer_tokenize(&lexer, "0b");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0b2");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0bA");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0bG");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0o");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0o8");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0x");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0xG");
	// 	EXPECT_FALSE(result.success);
	// })


	// TEST("Singledigit based integer", {
	// 	result = Lexer_tokenize(&lexer, "0b1");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0b1);
	// })


	// TEST("Multidigit based integer", {
	// 	result = Lexer_tokenize(&lexer, "0b10");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0b10);


	// 	result = Lexer_tokenize(&lexer, "0o123");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0123);


	// 	result = Lexer_tokenize(&lexer, "0x1aB");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0x1AB);
	// })


	// TEST("Trailing underscore", {
	// 	result = Lexer_tokenize(&lexer, "1_");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 1);


	// 	result = Lexer_tokenize(&lexer, "1____");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 1);


	// 	result = Lexer_tokenize(&lexer, "0.1_");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);


	// 	result = Lexer_tokenize(&lexer, "0_.1");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);


	// 	result = Lexer_tokenize(&lexer, "0_.1_");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);
	// })


	// TEST("Multiple underscores in a row", {
	// 	result = Lexer_tokenize(&lexer, "1____1");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 11);
	// })


	// TEST("Singledigit per underscore", {
	// 	result = Lexer_tokenize(&lexer, "1_1_1_1");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 1111);
	// })


	// TEST("Multiple digits per underscore", {
	// 	result = Lexer_tokenize(&lexer, "11_11_11");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 111111);
	// })


	// TEST("Invalid use of underscores", {
	// 	result = Lexer_tokenize(&lexer, "0o_");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0o_A");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0x_");
	// 	EXPECT_FALSE(result.success);

	// 	result = Lexer_tokenize(&lexer, "0x_1");
	// 	EXPECT_FALSE(result.success);
	// })


	// TEST("Trailing underscore in based literal", {
	// 	result = Lexer_tokenize(&lexer, "0x1_");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0x1);


	// 	result = Lexer_tokenize(&lexer, "0x1____");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0x1);
	// })


	// TEST("Singledigit per underscore in based literal", {
	// 	result = Lexer_tokenize(&lexer, "0x1_1_1");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0x111);
	// })


	// TEST("Named member accessor in integer literal", {
	// 	result = Lexer_tokenize(&lexer, "0.A");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "A"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0._");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "_"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0._A");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "_A"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0.field");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "field"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0x5.field");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_INTEGER);
	// 	EXPECT_EQUAL_INT(token->value.integer, 0x5);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "field"));
	// })

	TEST_BEGIN("Invalid digits after decimal point") {
		result = Lexer_tokenize(&lexer, "0.y");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0.");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0.3.");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0.3.9");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0.3.y");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, ".5");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, ".x");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, ".x5");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "2..5");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "2...5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		result = Lexer_tokenize(&lexer, "2..<5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);
	} TEST_END();

	// TEST("Named member accessor in float literal", {
	// 	result = Lexer_tokenize(&lexer, "0.5.A");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_INT(token->value.floating, 0.5);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "A"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0.5._");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_INT(token->value.floating, 0.5);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "_"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "1_.2_._");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_FLOAT(token->value.floating, 1.2);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "_"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0.5._A");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_INT(token->value.floating, 0.5);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "_A"));

	// 	//
	// 	result = Lexer_tokenize(&lexer, "0.5.field");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 4);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_INT(token->value.floating, 0.5);

	// 	token = (Token*)Array_get(lexer.tokens, 1);
	// 	EXPECT_TRUE(token->kind == TOKEN_DOT);

	// 	token = (Token*)Array_get(lexer.tokens, 2);
	// 	EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
	// 	EXPECT_TRUE(String_equals(token->value.identifier, "field"));
	// })


	// TEST("Trailing underscore in float literal", {
	// 	result = Lexer_tokenize(&lexer, "0.1____");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);
	// })


	// TEST("Singledigit per underscore in float literal", {
	// 	result = Lexer_tokenize(&lexer, "0.1_1_1_1");
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);
	// 	EXPECT_TRUE(token->kind == TOKEN_FLOATING);
	// 	EXPECT_EQUAL_FLOAT(token->value.floating, 0.1111);
	// })


	// TODO: Test exponents
	TEST("General exponent use in integer literal", {
		result = Lexer_tokenize(&lexer, "10e5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10e5);


		result = Lexer_tokenize(&lexer, "10e+5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10e+5);


		result = Lexer_tokenize(&lexer, "10E-5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10E-5);


		result = Lexer_tokenize(&lexer, "10e+0");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10e+0);


		result = Lexer_tokenize(&lexer, "1_0e+5_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 6);
	})

	TEST("General exponent use in float literal", {
		result = Lexer_tokenize(&lexer, "0.1e5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.1e5);


		result = Lexer_tokenize(&lexer, "0.1e+5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.1e+5);


		result = Lexer_tokenize(&lexer, "0.1E-5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.1E-5);


		result = Lexer_tokenize(&lexer, "0.1e+0");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.1e+0);


		result = Lexer_tokenize(&lexer, "0.1_e+5_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 6);
	})

	TEST("Singledigit/Multidigit exponent in singledigit/multidigit integer literal", {
		result = Lexer_tokenize(&lexer, "1e5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 1e5);


		result = Lexer_tokenize(&lexer, "1e10");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 1e10);


		result = Lexer_tokenize(&lexer, "10e5");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10e5);


		result = Lexer_tokenize(&lexer, "10e10");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10e10);
	})

	TEST("Invalid use of exponent", {
		result = Lexer_tokenize(&lexer, "10e");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e-");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+_");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e-_");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+5e");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "10e+5e3");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "10e+5e+3");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 5);

		result = Lexer_tokenize(&lexer, "10e+5a");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, "10e+5_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);
	})
}

DESCRIBE(boolean_tokenization, "Boolean literals tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("True", {
		result = Lexer_tokenize(&lexer, "true");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_BOOLEAN);
		EXPECT_TRUE(token->value.boolean);
	})

	TEST("False", {
		result = Lexer_tokenize(&lexer, "false");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_BOOLEAN);
		EXPECT_FALSE(token->value.boolean);
	})
}

DESCRIBE(nil_tokenization, "Nil literal tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Simple Nil", {
		result = Lexer_tokenize(&lexer, "nil");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_NIL);
	})
}

DESCRIBE(string_tokenization, "String literals tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Empty string", {
		result = Lexer_tokenize(&lexer, "\"\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, ""));
		EXPECT_EQUAL_INT(token->value.string->length, 0);
	})

	TEST("Single character string", {
		result = Lexer_tokenize(&lexer, "\"a\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "a"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multicharacter string", {
		result = Lexer_tokenize(&lexer, "\"abc\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "abc"));
		EXPECT_EQUAL_INT(token->value.string->length, 3);
	})
}

DESCRIBE(ml_string_token_single, "Multiline string literals tokenization on a single line") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST_BEGIN("Empty string") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"" LF
			"\"\"\"" LF
		);
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, ""));
		EXPECT_EQUAL_INT(token->value.string->length, 0);
	} TEST_END();

	TEST_BEGIN("Single character string") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"" LF
			"  Aaa" LF
			"  \"\"\"" LF
		);
		// result = Lexer_tokenize(
		// 	&lexer,
		// 	"\"\"\"" LF
		// 	"A" LF
		// 	"\"\"\"" LF
		// );
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "Aaa"));
		EXPECT_EQUAL_INT(token->value.string->length, 3);
	} TEST_END();

	TEST_BEGIN("Single character string") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"" LF
			"Hello!" LF
			"\"\"\"" LF
		);
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "Hello!"));
		EXPECT_EQUAL_INT(token->value.string->length, 6);
	} TEST_END();
}

DESCRIBE(ml_string_token_multi, "Multiline string literals tokenization on a single line") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST_BEGIN("Empty string containing a newline") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"\"\"\"" LF
		);
		EXPECT_FALSE(result.success);
	} TEST_END();

	TEST_BEGIN("Single character string on a single line") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"A" LF
			"\"\"\"" LF
		);
		EXPECT_FALSE(result.success);
	} TEST_END();

	TEST_BEGIN("Single character string on a new line") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"" LF
			"A\"\"\"" LF
		);
		EXPECT_FALSE(result.success);
	} TEST_END();

	TEST_BEGIN("Single character string") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"A" LF
			"B" LF
			"C\"\"\"" LF
		);
		EXPECT_FALSE(result.success);
	} TEST_END();
}

DESCRIBE(ml_string_token_escape, "Multiline string literals tokenization with escape sequences") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	// TEST_BEGIN("Escaped newline") {
	// 	result = Lexer_tokenize(
	// 		&lexer,
	// 		"\"\"\"" LF
	// 		"\\n" LF
	// 		"\"\"\"" LF
	// 	);
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);

	// 	EXPECT_TRUE(token->kind == TOKEN_STRING);
	// 	EXPECT_TRUE(String_equals(token->value.string, "\n"));
	// 	EXPECT_EQUAL_INT(token->value.string->length, 1);
	// } TEST_END();

	// TEST_BEGIN("Hexadecimal escape sequence") {
	// 	result = Lexer_tokenize(
	// 		&lexer,
	// 		"\"\"\"" LF
	// 		"\\u{00041}" LF
	// 		"\"\"\"" LF
	// 	);
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);

	// 	EXPECT_TRUE(token->kind == TOKEN_STRING);
	// 	EXPECT_TRUE(String_equals(token->value.string, "\nA\n"));
	// 	EXPECT_EQUAL_INT(token->value.string->length, 3);
	// } TEST_END();

	TEST_BEGIN("Single escaped double quote") {
		result = Lexer_tokenize(
			&lexer,
			"\"\"\"" LF
			"\"" LF
			"\"\"\"" LF
		);
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\""));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	} TEST_END();

	// TEST_BEGIN("Single character string") {
	// 	result = Lexer_tokenize(
	// 		&lexer,
	// 		"\"\"\"" LF
	// 		"A" LF
	// 		"B" LF
	// 		"C" LF
	// 		"\"\"\"" LF
	// 	);
	// 	EXPECT_TRUE(result.success);
	// 	EXPECT_EQUAL_INT(lexer.tokens->size, 2);

	// 	token = (Token*)Array_get(lexer.tokens, 0);

	// 	EXPECT_TRUE(token->kind == TOKEN_STRING);
	// 	EXPECT_TRUE(String_equals(token->value.string, "A\nB\nC"));
	// 	EXPECT_EQUAL_INT(token->value.string->length, 5);
	// } TEST_END();
}

DESCRIBE(string_invalid_tokeniz, "Invalid string literals tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST("Unterminated string", {
		result = Lexer_tokenize(&lexer, "\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Unescaped quote", {
		result = Lexer_tokenize(&lexer, "\"Hello \" World\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Unescaped backslash", {
		result = Lexer_tokenize(&lexer, "\"Hello \\ World\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Unescaped newline", {
		result = Lexer_tokenize(&lexer, "\"Hello \n World\"");
		EXPECT_FALSE(result.success);
	})
}

DESCRIBE(string_quotes_escape, "String literals tokenization with escaped quotes") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Single escaped double quote", {
		result = Lexer_tokenize(&lexer, "\"\\\"\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\""));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multiple escaped double quotes", {
		result = Lexer_tokenize(&lexer, "\"\\\"\\\"\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\"\""));
		EXPECT_EQUAL_INT(token->value.string->length, 2);
	})

	TEST("Multiple escaped double quotes with text", {
		result = Lexer_tokenize(&lexer, "\"pre \\\"in\\\" post\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre \"in\" post"));
		EXPECT_EQUAL_INT(token->value.string->length, 13);
	})
}

DESCRIBE(string_backslash_escape, "String literals tokenization with escaped backslashes") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Single escaped backslash", {
		result = Lexer_tokenize(&lexer, "\"\\\\\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\\"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multiple escaped backslashes", {
		result = Lexer_tokenize(&lexer, "\"\\\\\\\\\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\\\\"));
		EXPECT_EQUAL_INT(token->value.string->length, 2);
	})

	TEST("Multiple escaped backslashes with text", {
		result = Lexer_tokenize(&lexer, "\"pre \\\\in\\\\ post\"");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre \\in\\ post"));
		EXPECT_EQUAL_INT(token->value.string->length, 13);
	})
}

DESCRIBE(string_whitespace_escape, "String literals tokenization with escaped whitespace") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Single line feed", {
		result = Lexer_tokenize(&lexer, "\"\\n\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\n"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multiple line feeds", {
		result = Lexer_tokenize(&lexer, "\"\\n\\n\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\n\n"));
		EXPECT_EQUAL_INT(token->value.string->length, 2);
	})

	TEST_BEGIN("Line feed as escaped char") {
		result = Lexer_tokenize(&lexer, "\"line1 \\\nline2\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "line1 \nline2"));
		EXPECT_EQUAL_INT(token->value.string->length, 12);
	} TEST_END();


	TEST("Single carriage return", {
		result = Lexer_tokenize(&lexer, "\"\\r\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\r"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multiple carriage returns", {
		result = Lexer_tokenize(&lexer, "\"\\r\\r\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\r\r"));
		EXPECT_EQUAL_INT(token->value.string->length, 2);
	})


	TEST("Single tab", {
		result = Lexer_tokenize(&lexer, "\"\\t\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\t"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multiple tabs", {
		result = Lexer_tokenize(&lexer, "\"\\t\\t\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\t\t"));
		EXPECT_EQUAL_INT(token->value.string->length, 2);
	})
}

DESCRIBE(string_unicode_escape, "String literals tokenization with escaped unicode sequences") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Single unicode character", {
		result = Lexer_tokenize(&lexer, "\"\\u{61}\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "a"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Single prefixed unicode character", {
		result = Lexer_tokenize(&lexer, "\"\\u{0061}\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "a"));
		EXPECT_EQUAL_INT(token->value.string->length, 1);
	})

	TEST("Multiple unicode characters", {
		result = Lexer_tokenize(&lexer, "\"\\u{61}\\u{62}\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "ab"));
		EXPECT_EQUAL_INT(token->value.string->length, 2);
	})

	TEST("Multiple unicode characters in text", {
		result = Lexer_tokenize(&lexer, "\"pre \\u{61} in \\u{62} post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre a in b post"));
		EXPECT_EQUAL_INT(token->value.string->length, 15);
	})
}

DESCRIBE(str_invalid_escape, "Invalid escape sequences in string literals tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST("Invalid escape sequence", {
		result = Lexer_tokenize(&lexer, "\"\\a\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Incomplete unicode sequence 1", {
		result = Lexer_tokenize(&lexer, "\"\\u\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Incomplete unicode sequence 2", {
		result = Lexer_tokenize(&lexer, "\"\\u{\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Incomplete unicode sequence 3", {
		result = Lexer_tokenize(&lexer, "\"\\u{61\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Too short unicode sequence", {
		result = Lexer_tokenize(&lexer, "\"\\u{}\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Too long unicode sequence", {
		result = Lexer_tokenize(&lexer, "\"\\u{123456789}\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Out of the range unicode sequence", {
		result = Lexer_tokenize(&lexer, "\"\\u{FFFFFFFF}\"");
		EXPECT_FALSE(result.success);
	})

	TEST("Invalid characters in unicode sequence", {
		result = Lexer_tokenize(&lexer, "\"\\u{FFXF}\"");
		EXPECT_FALSE(result.success);
	})
}

DESCRIBE(string_escape_sequences, "String literals tokenization with escaped sequences") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Combination of all supported escape sequences", {
		result = Lexer_tokenize(&lexer, "\"\\\"\\\\\\n\\r\\t\\u{61}\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);

		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "\"\\\n\r\ta"));
		EXPECT_EQUAL_INT(token->value.string->length, 6);
	})
}

DESCRIBE(multiple_tokens_togeth, "Multiple tokens together") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	// Token *token;

	TEST_BEGIN("IDK") {
		result = Lexer_tokenize(&lexer, "&&&&");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 3);

		result = Lexer_tokenize(&lexer, ".");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "..");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "...");
		EXPECT_TRUE(result.success);

		result = Lexer_tokenize(&lexer, "%%");
		EXPECT_FALSE(result.success);
	} TEST_END()
}

DESCRIBE(string_interpolation, "Interpolated string literal tokenization") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	Token *token;

	TEST("Simple interpolated string", {
		result = Lexer_tokenize(&lexer, "\"Hello \\(name)!\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "Hello "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "name"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "!"));
	})

	TEST_BEGIN("Interpolated string starts with expression") {
		result = Lexer_tokenize(&lexer, "\"\\(expr) post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, ""));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	} TEST_END()

	TEST("Interpolated string ends with expression", {
		result = Lexer_tokenize(&lexer, "\"pre \\(expr)\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, ""));
	})

	TEST("Interpolation containing whitespace", {
		result = Lexer_tokenize(&lexer, "\"pre \\(\n\texpr\n) post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	})

	TEST("Interpolation containing comment", {
		result = Lexer_tokenize(&lexer, "\"pre \\(/*comment*/\nexpr\n/*hi!*/\n) post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	})

	TEST("Interpolated string starts & ends with expression", {
		result = Lexer_tokenize(&lexer, "\"\\(expr)\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, ""));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, ""));
	})

	TEST("Interpolated string containing multiple interpolations", {
		result = Lexer_tokenize(&lexer, "\"pre \\(expr1) in \\(expr2) post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr1"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_SPAN);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " in "));

		token = (Token*)Array_get(lexer.tokens, 5);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 6);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr2"));

		token = (Token*)Array_get(lexer.tokens, 7);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 8);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	})

	TEST("Interpolated string contining parentheses in expression", {
		result = Lexer_tokenize(&lexer, "\"pre \\(expr * (a + b)) post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_OPERATOR);
		EXPECT_TRUE(token->kind == TOKEN_STAR);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->type == TOKEN_PUNCTUATOR);
		EXPECT_TRUE(token->kind == TOKEN_LEFT_PAREN);

		token = (Token*)Array_get(lexer.tokens, 5);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "a"));

		token = (Token*)Array_get(lexer.tokens, 6);
		EXPECT_TRUE(token->type == TOKEN_OPERATOR);
		EXPECT_TRUE(token->kind == TOKEN_PLUS);

		token = (Token*)Array_get(lexer.tokens, 7);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "b"));

		token = (Token*)Array_get(lexer.tokens, 8);
		EXPECT_TRUE(token->type == TOKEN_PUNCTUATOR);
		EXPECT_TRUE(token->kind == TOKEN_RIGHT_PAREN);

		token = (Token*)Array_get(lexer.tokens, 9);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 10);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	})

	TEST("Interpolated string contining string as teh expression", {
		result = Lexer_tokenize(&lexer, "\"pre \\(\"in\") post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "in"));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	})

	TEST("Nested interpolated strings", {
		result = Lexer_tokenize(&lexer, "\"pre \\(\"in_pre \\(expr) in_post\") post\"");
		EXPECT_TRUE(result.success);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "pre "));

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, "in_pre "));

		token = (Token*)Array_get(lexer.tokens, 3);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_HEAD);

		token = (Token*)Array_get(lexer.tokens, 4);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "expr"));

		token = (Token*)Array_get(lexer.tokens, 5);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 6);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " in_post"));

		token = (Token*)Array_get(lexer.tokens, 7);
		EXPECT_TRUE(token->type == TOKEN_STRING_INTERPOLATION_MARKER);
		EXPECT_TRUE(token->kind == TOKEN_STRING_TAIL);

		token = (Token*)Array_get(lexer.tokens, 8);
		EXPECT_TRUE(token->kind == TOKEN_STRING);
		EXPECT_TRUE(String_equals(token->value.string, " post"));
	})

	TEST("Invalid use interpolated strings", {
		result = Lexer_tokenize(&lexer, "\"\\(\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\(\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\( post\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"\\( post\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\(\"\"\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\(\"\" post\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\(\"in\" post\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\(\"in_pre \\(expr)\" post\"");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "\"pre \\(\"in_pre \\(expr\") post\"");
		EXPECT_FALSE(result.success);
	})
}

DESCRIBE(nextToken, "Token stream (nextToken)") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST_BEGIN("Tokenization of the complex source") {
		Lexer_setSource(&lexer, "var myVar = myFunc(/* some params */)");

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_KEYWORD);
		EXPECT_TRUE(result.token->kind == TOKEN_VAR);
		EXPECT_TRUE(result.token->whitespace == (WHITESPACE_NONE | WHITESPACE_RIGHT_SPACE));

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(result.token->value.identifier, "myVar"));
		EXPECT_TRUE(result.token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_OPERATOR);
		EXPECT_TRUE(result.token->kind == TOKEN_EQUAL);
		EXPECT_TRUE(result.token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_RIGHT_SPACE));

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(result.token->value.identifier, "myFunc"));
		EXPECT_TRUE(result.token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_NONE));

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_PUNCTUATOR);
		EXPECT_TRUE(result.token->kind == TOKEN_LEFT_PAREN);
		EXPECT_TRUE(result.token->whitespace == (WHITESPACE_NONE | WHITESPACE_RIGHT_SPACE));

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_PUNCTUATOR);
		EXPECT_TRUE(result.token->kind == TOKEN_RIGHT_PAREN);
		EXPECT_TRUE(result.token->whitespace == (WHITESPACE_LEFT_SPACE | WHITESPACE_NONE));

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);
	} TEST_END();

	TEST_BEGIN("Next token from the empty source") {
		Lexer_setSource(&lexer, "");

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);
	} TEST_END();

	TEST_BEGIN("Next token after EOF") {
		Lexer_setSource(&lexer, "10");

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 10);

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);
	} TEST_END();
}

DESCRIBE(peekToken, "Peeking tokens (peekToken)") {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

	TEST_BEGIN("Combination of nextToken and peekToken") {
		Lexer_setSource(&lexer, "1 2 3 4 5 6 7 8");

		//

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 1);

		// 1
		// ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 1);

		// 1
		// ^

		result = Lexer_peekToken(&lexer, 1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 2);

		// 1 2
		// ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 2);

		// 1 2
		//   ^

		result = Lexer_peekToken(&lexer, -1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 1);

		// 1 2
		//   ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 2);

		// 1 2
		//   ^

		result = Lexer_peekToken(&lexer, 1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 3);

		// 1 2 3
		//   ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 3);

		// 1 2 3
		//     ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 3);

		// 1 2 3
		//     ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 4);

		// 1 2 3 4
		//       ^

		result = Lexer_peekToken(&lexer, -1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 3);

		// 1 2 3 4
		//       ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 4);

		// 1 2 3 4

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 5);

		// 1 2 3 4 5
		//         ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 6);

		// 1 2 3 4 5 6
		//           ^

		result = Lexer_peekToken(&lexer, -1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 5);

		// 1 2 3 4 5 6
		//           ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 6);

		// 1 2 3 4 5 6
		//		     ^

		result = Lexer_peekToken(&lexer, 2);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 8);

		// 1 2 3 4 5 6 7 8
		//		     ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 7);

		// 1 2 3 4 5 6 7 8
		//		       ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 8);

		// 1 2 3 4 5 6 7 8
		//		         ^

		result = Lexer_peekToken(&lexer, -1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);
		EXPECT_TRUE(result.token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(result.token->value.integer, 7);

		// 1 2 3 4 5 6 7 8
		//		         ^

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

	} TEST_END();

	TEST_BEGIN("Tokenization of the complex source") {
		Lexer_setSource(&lexer, "var myVar = myFunc(/* some params */)");

		//

		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_KEYWORD);
		EXPECT_TRUE(result.token->kind == TOKEN_VAR);

		// var
		// ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_KEYWORD);
		EXPECT_TRUE(result.token->kind == TOKEN_VAR);

		// var
		// ^

		result = Lexer_peekToken(&lexer, 1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(result.token->value.identifier, "myVar"));

		// var myVar
		// ^

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_KEYWORD);
		EXPECT_TRUE(result.token->kind == TOKEN_VAR);

		// var myVar
		// ^

		result = Lexer_peekToken(&lexer, -1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token == NULL);

		// var myVar
		// ^

		result = Lexer_peekToken(&lexer, -2);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token == NULL);

		// var myVar
		// ^

		result = Lexer_peekToken(&lexer, 2);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_OPERATOR);
		EXPECT_TRUE(result.token->kind == TOKEN_EQUAL);

		// var myVar =
		// ^

		result = Lexer_peekToken(&lexer, 3);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(result.token->value.identifier, "myFunc"));

		// var myVar = myFunc
		// ^

		result = Lexer_peekToken(&lexer, 5);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_PUNCTUATOR);
		EXPECT_TRUE(result.token->kind == TOKEN_RIGHT_PAREN);

		// var myVar = myFunc();
		// ^

		result = Lexer_peekToken(&lexer, 4);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_PUNCTUATOR);
		EXPECT_TRUE(result.token->kind == TOKEN_LEFT_PAREN);

		// var myVar = myFunc();
		// ^

		result = Lexer_peekToken(&lexer, 7);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

		// var myVar = myFunc();

		result = Lexer_peekToken(&lexer, 8);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

	} TEST_END();

	TEST_BEGIN("Next token from the empty source") {
		Lexer_setSource(&lexer, "");

		EXPECT_TRUE(Lexer_isAtEnd(&lexer));

		result = Lexer_peekToken(&lexer, 1);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

		EXPECT_TRUE(Lexer_isAtEnd(&lexer));
	} TEST_END();

	TEST_BEGIN("Next token after EOF") {
		Lexer_setSource(&lexer, "10");

		EXPECT_FALSE(Lexer_isAtEnd(&lexer));

		result = Lexer_peekToken(&lexer, 0);
		EXPECT_TRUE(result.success);
		EXPECT_NULL(result.token);

		EXPECT_FALSE(Lexer_isAtEnd(&lexer));

		result = Lexer_peekToken(&lexer, 1);
		EXPECT_TRUE(result.success);
		EXPECT_NOT_NULL(result.token);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);

		EXPECT_TRUE(Lexer_isAtEnd(&lexer));

		result = Lexer_peekToken(&lexer, 2);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

		EXPECT_TRUE(Lexer_isAtEnd(&lexer));

		result = Lexer_peekToken(&lexer, 3);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_EOF);

		EXPECT_TRUE(Lexer_isAtEnd(&lexer));

		// Now consume the token
		result = Lexer_nextToken(&lexer);
		EXPECT_TRUE(result.success);
		EXPECT_TRUE(result.token->type == TOKEN_LITERAL);

		EXPECT_TRUE(Lexer_isAtEnd(&lexer));
	} TEST_END();
}
