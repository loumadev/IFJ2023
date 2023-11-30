#include <stdio.h>

#include "unit.h"
#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Parser.h"
#include "compiler/lexer/Token.h"
#include "compiler/parser/ExpressionParser.h"
#include "allocator/MemoryAllocator.h"
#include "../parser/parser_assertions.h"

#define TEST_PRIORITY 80
#define STACK_SIZE 20

DESCRIBE(epxr_parser_internal, "Expression parser internal functions"){
    StackItem *topTerminal = mem_alloc(sizeof(StackItem));
    StackItem *dollarTerminal = mem_alloc(sizeof(StackItem));
    StackItem *stopItem = mem_alloc(sizeof(StackItem));
    dollarTerminal->Stype = S_BOTTOM;
    Array *stack = Array_alloc(STACK_SIZE);

    Array_push(stack, dollarTerminal);


    TEST_BEGIN("one terminal on stack"){
        topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_BOTTOM);
    }TEST_END();

    TEST_BEGIN("two terminals on stack"){
        StackItem *secondTerminal = mem_alloc(sizeof(StackItem));
        secondTerminal->Stype = S_TERMINAL;
        Array_push(stack, secondTerminal);
        topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_TERMINAL);
    }TEST_END();


    TEST_BEGIN("one nonterminal, two terminals on stack"){
        StackItem *nonTerminal = mem_alloc(sizeof(StackItem));
        nonTerminal->Stype = S_NONTERMINAL;
        Array_push(stack, nonTerminal);
        topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_TERMINAL);
        Array_pop(stack);
        Array_pop(stack);
    }TEST_END();

    TEST_BEGIN("push after first terminal on stack"){
        Expr_pushAfterTopTerminal(stack);
        stopItem = (StackItem*)Array_get(stack, 1);
        EXPECT_TRUE(stopItem->Stype == S_STOP);
        Array_pop(stack);
    }TEST_END();

    TEST_BEGIN("push after second terminal on stack"){
        stopItem->Stype = S_NONTERMINAL;
        StackItem *secondTerminal = mem_alloc(sizeof(StackItem));
        secondTerminal->Stype = S_TERMINAL;
        Array_push(stack, secondTerminal);
        Expr_pushAfterTopTerminal(stack);
        stopItem = (StackItem*)Array_get(stack, 2);
        EXPECT_TRUE(stopItem->Stype == S_STOP);
        Array_pop(stack);
    }TEST_END();

     TEST_BEGIN("overcome nonterminal and push after top terminal"){        
        StackItem *nonTerminal = mem_alloc(sizeof(StackItem));
        nonTerminal->Stype = S_NONTERMINAL;
        Array_push(stack, nonTerminal);
        Expr_pushAfterTopTerminal(stack);
        stopItem = (StackItem*)Array_get(stack, 2);
        EXPECT_TRUE(stopItem->Stype == S_STOP);

    }TEST_END();

    StackItem *terminal = mem_alloc(sizeof(StackItem));
    StackItem *result = mem_alloc(sizeof(StackItem));
    Array *reductionStack = Array_alloc(STACK_SIZE);

    TEST_BEGIN("reducton of literal"){
        Token *intLiteral = mem_alloc(sizeof(Token));
        intLiteral->type = TOKEN_LITERAL;
        intLiteral->value.floating = 0.9;
        terminal->token = intLiteral;
        terminal->Stype = S_TERMINAL;
        terminal->node = NULL;
        Array_push(reductionStack, terminal);
        result = Expr_performReduction(reductionStack);
        EXPECT_TRUE(result->node->_type == NODE_LITERAL_EXPRESSION);        
        EXPECT_TRUE(result->Stype = S_NONTERMINAL);
    }TEST_END();

}

DESCRIBE(expressions, "Expression parsing"){
    Lexer lexer;
	Lexer_constructor(&lexer);

	Parser parser;
	Parser_constructor(&parser, &lexer);

	ParserResult result;

    TEST_BEGIN("parse expression"){
        Lexer_setSource(&lexer, "let a = 7-");
        result = Parser_parse(&parser);

        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "let a = 7- " LF
                                "2 " LF
                                "b = 8"
             );
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "x+y {" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "x+y+(" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "x+y y" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "x+y a=" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "x+y)" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "str1 = readString() ?? \"\"" );
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "write(\"Spatne zadana posloupnost, zkuste znovu:\\n\")");
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "write(str1, \"\\n\", str2, \"\\n\")");
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "write(\"Pozice retezce \\\"text\\\" v str2: \", i, \"\\n\")");
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "str1 = tmp ?? \"\"");
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "var str1 = \"Toto je nejaky text v programu jazyka IFJ23\"" LF
                                "let str2 = str1 + \", ktery jeste trochu obohatime\"" LF
                                "write(str1, \"\\n\", str2, \"\\n\")" LF
                                "let i = length(str1)" LF
                                "write(\"Pozice retezce \\\"text\\\" v str2: \", i, \"\\n\")" LF
                                "write(\"Zadejte serazenou posloupnost vsech malych pismen a-h, \")" LF
                                "let newInput = readString()" LF
                                " if let newInput {" LF
                                 TAB   "str1 = newInput" LF
                                 TAB TAB   "while (str1 != \"abcdefgh\") {" LF
                                 TAB TAB TAB   "write(\"Spatne zadana posloupnost, zkuste znovu:\\n\")" LF
                                 TAB TAB TAB   "let tmp = readString()" LF
                                 TAB TAB TAB   "str1 = tmp ?? \"\"" LF
                                "}" LF
                                "} else {}" LF);
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "(x if" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "(+ a =" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "a!(a*b)" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "a(a*b)" );
        result = Parser_parse(&parser);
        EXPECT_TRUE(result.success);

        Lexer_setSource(&lexer, "a!(a*b)\"string\"" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "a!(a*b)\\\"string\\\"" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "+" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);
        
        Lexer_setSource(&lexer, "a<=!=b" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);

        Lexer_setSource(&lexer, "!a-b!" );
        result = Parser_parse(&parser);
        EXPECT_FALSE(result.success);
    }TEST_END();

    
}