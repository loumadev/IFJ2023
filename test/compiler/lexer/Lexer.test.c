#include "compiler/lexer/Lexer.h"
#include "unit.h"
#include <stdio.h>

void comment_stripping() {
	Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;
	// Token *token;

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

void numbers_tokenization() {
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


	TEST("Leading dot in float literal", {
		result = Lexer_tokenize(&lexer, ".5");
		EXPECT_FALSE(result.success);
	})

	TEST("Invalid digits in integer literal", {
		result = Lexer_tokenize(&lexer, "1y");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10y");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10y6");
		EXPECT_FALSE(result.success);
	})

	TEST("Invalid digits in float literal", {
		result = Lexer_tokenize(&lexer, "1.0y");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "1y.0");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10.20y");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10y.20");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10y.20y6");
		EXPECT_FALSE(result.success);
	})


	TEST("Invalid digits in based integer literal", {
		result = Lexer_tokenize(&lexer, "0b");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0b2");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0bA");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0bG");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0o");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0o8");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0x");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0xG");
		EXPECT_FALSE(result.success);
	})


	TEST("Singledigit based integer", {
		result = Lexer_tokenize(&lexer, "0b1");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0b1);
	})


	TEST("Multidigit based integer", {
		result = Lexer_tokenize(&lexer, "0b10");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0b10);


		result = Lexer_tokenize(&lexer, "0o123");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0123);


		result = Lexer_tokenize(&lexer, "0x1aB");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0x1AB);
	})


	TEST("Trailing underscore", {
		result = Lexer_tokenize(&lexer, "1_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 1);


		result = Lexer_tokenize(&lexer, "1____");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 1);


		result = Lexer_tokenize(&lexer, "0.1_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);


		result = Lexer_tokenize(&lexer, "0_.1");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);


		result = Lexer_tokenize(&lexer, "0_.1_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);
	})


	TEST("Multiple underscores in a row", {
		result = Lexer_tokenize(&lexer, "1____1");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 11);
	})


	TEST("Singledigit per underscore", {
		result = Lexer_tokenize(&lexer, "1_1_1_1");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 1111);
	})


	TEST("Multiple digits per underscore", {
		result = Lexer_tokenize(&lexer, "11_11_11");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 111111);
	})


	TEST("Invalid use of underscores", {
		result = Lexer_tokenize(&lexer, "0o_");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0o_A");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0x_");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "0x_1");
		EXPECT_FALSE(result.success);
	})


	TEST("Trailing underscore in based literal", {
		result = Lexer_tokenize(&lexer, "0x1_");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0x1);


		result = Lexer_tokenize(&lexer, "0x1____");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0x1);
	})


	TEST("Singledigit per underscore in based literal", {
		result = Lexer_tokenize(&lexer, "0x1_1_1");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0x111);
	})


	TEST("Named member accessor in integer literal", {
		result = Lexer_tokenize(&lexer, "0.A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "A"));

		//
		result = Lexer_tokenize(&lexer, "0._");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "_"));

		//
		result = Lexer_tokenize(&lexer, "0._A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "_A"));

		//
		result = Lexer_tokenize(&lexer, "0.field");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "field"));

		//
		result = Lexer_tokenize(&lexer, "0x5.field");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_INTEGER);
		EXPECT_EQUAL_INT(token->value.integer, 0x5);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "field"));
	})

	TEST("Named member accessor in float literal", {
		result = Lexer_tokenize(&lexer, "0.5.A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.5);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "A"));

		//
		result = Lexer_tokenize(&lexer, "0.5._");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.5);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "_"));

		//
		result = Lexer_tokenize(&lexer, "1_.2_._");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 1.2);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "_"));

		//
		result = Lexer_tokenize(&lexer, "0.5._A");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.5);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "_A"));

		//
		result = Lexer_tokenize(&lexer, "0.5.field");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 4);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.5);

		token = (Token*)Array_get(lexer.tokens, 1);
		EXPECT_TRUE(token->kind == TOKEN_DOT);

		token = (Token*)Array_get(lexer.tokens, 2);
		EXPECT_TRUE(token->type == TOKEN_IDENTIFIER);
		EXPECT_TRUE(String_equals(token->value.identifier, "field"));
	})


	TEST("Trailing underscore in float literal", {
		result = Lexer_tokenize(&lexer, "0.1____");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 0.1);
	})


	TEST("Singledigit per underscore in float literal", {
		result = Lexer_tokenize(&lexer, "0.1_1_1_1");
		EXPECT_TRUE(result.success);
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_FLOAT(token->value.floating, 0.1111);
	})


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
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 10e+5);
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
		EXPECT_EQUAL_INT(lexer.tokens->size, 2);

		token = (Token*)Array_get(lexer.tokens, 0);
		EXPECT_TRUE(token->kind == TOKEN_FLOATING);
		EXPECT_EQUAL_INT(token->value.floating, 0.1e+5);
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
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+5e3");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+5e+3");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+5a");
		EXPECT_FALSE(result.success);

		result = Lexer_tokenize(&lexer, "10e+5_");
		EXPECT_TRUE(result.success);
	})

	if(result.success) {
		// printf("Success!\n");

		// Lexer_printTokens(&lexer);
	} else {
		printf("Failure!\n");
		printf(RED "Error: %s\n" RST, result.message->value);
	}
}
