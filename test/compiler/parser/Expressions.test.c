#include "unit.h"
#include "compiler/parser/Expressions.h"
#include "allocator/MemoryAllocator.h"
#include "internal/Array.h"
#include "internal/String.h"
#include "inspector.h"
#include "compiler/lexer/Lexer.h"
#include <stdio.h>

#define STACK_SIZE 20

DESCRIBE(stack_top_terminal, "Get top terminal from stack"){
    StackItem *topTerminal = mem_alloc(sizeof(StackItem));
    StackItem *dollarTerminal = mem_alloc(sizeof(StackItem));
    dollarTerminal->Stype = S_BOTTOM;
    Array *stack = Array_alloc(STACK_SIZE);
   
    Array_push(stack, dollarTerminal);
    

    TEST_BEGIN("one terminal on stack"){
        *topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_BOTTOM);
    }TEST_END();

    TEST_BEGIN("two terminals on stack"){
        StackItem *secondTerminal = mem_alloc(sizeof(StackItem));
        secondTerminal->Stype = S_TERMINAL;
        Array_push(stack, secondTerminal);
        *topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_TERMINAL);
    }TEST_END();

    
    TEST_BEGIN("one nonterminal, two terminals on stack"){
        StackItem *nonTerminal = mem_alloc(sizeof(StackItem));
        nonTerminal->Stype = S_NONTERMINAL;
        Array_push(stack, nonTerminal);
        *topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_TERMINAL);
    }TEST_END();

}

DESCRIBE(stack_push_after_top_t, "Push after top terminal on stack"){
    StackItem *stopItem = mem_alloc(sizeof(StackItem));
    StackItem *dollarTerminal = mem_alloc(sizeof(StackItem));
    dollarTerminal->Stype = S_BOTTOM;
    Array *stack = Array_alloc(STACK_SIZE);
   
    Array_push(stack, dollarTerminal);
    
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
  

}

DESCRIBE(reduction, "performs reductions according to grammar"){
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

    //TODO: reduction of identifier

    TEST_BEGIN("reducton of E! "){
        StackItem *operatorExc = mem_alloc(sizeof(StackItem));
        Token *operatorUnwrap = mem_alloc(sizeof(Token));
        operatorUnwrap->type = TOKEN_OPERATOR;
        operatorUnwrap->kind = TOKEN_EXCLAMATION;
        operatorExc->Stype = S_TERMINAL;
        operatorExc->token = operatorUnwrap;

        Array_push(reductionStack, terminal);
        Array_push(reductionStack, operatorExc);
        
        result = Expr_performReduction(reductionStack);
        EXPECT_TRUE(result->node->_type == NODE_UNARY_EXPRESSION);
        EXPECT_TRUE(result->Stype = S_NONTERMINAL);

    }TEST_END();

    TEST_BEGIN("reduction of E + E "){
        StackItem *operator = mem_alloc(sizeof(StackItem));
        Token *operatorPlus = mem_alloc(sizeof(Token));
        operatorPlus->type = TOKEN_OPERATOR;
        operatorPlus->kind = TOKEN_PLUS;
        operator->Stype = S_TERMINAL;
        operator->token = operatorPlus;

        Array_push(reductionStack, terminal);
        Array_push(reductionStack, operator);
        Array_push(reductionStack, terminal);

        result = Expr_performReduction(reductionStack);
        EXPECT_TRUE(result->node->_type == NODE_BINARY_EXPRESSION);
        EXPECT_TRUE(result->Stype = S_NONTERMINAL);

    }TEST_END();

    TEST_BEGIN("reduction of (E) "){
        StackItem *opLeftParen = mem_alloc(sizeof(StackItem));
        Token *leftparen = mem_alloc(sizeof(Token));
        leftparen->type = TOKEN_OPERATOR;
        leftparen->kind = TOKEN_LEFT_PAREN;
        opLeftParen->Stype = S_TERMINAL;
        opLeftParen->token = leftparen;

        StackItem *opRightParen = mem_alloc(sizeof(StackItem));
        Token *rigtparen = mem_alloc(sizeof(Token));
        rigtparen->type = TOKEN_OPERATOR;
        rigtparen->kind = TOKEN_RIGHT_PAREN;
        opRightParen->Stype = S_TERMINAL;
        opRightParen->token = rigtparen;

        Array_push(reductionStack, opLeftParen);
        Array_push(reductionStack, terminal);
        Array_push(reductionStack, opRightParen);
        result = Expr_performReduction(reductionStack);
        EXPECT_TRUE(result->node->_type == NODE_LITERAL_EXPRESSION);
        EXPECT_TRUE(result->Stype = S_NONTERMINAL);

    }TEST_END();
    
}

DESCRIBE(parse_expression, "parse expression"){
    Lexer lexer;
	Lexer_constructor(&lexer);

	LexerResult result;

    TEST_BEGIN("parse single character expression"){
        result = Lexer_tokenize(&lexer, "a ");
    }TEST_END();
}