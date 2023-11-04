#include "unit.h"
#include "compiler/parser/Expressions.h"
#include "allocator/MemoryAllocator.h"
#include "internal/Array.h"
#include "internal/String.h"
#include "inspector.h"

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
        StackItem *secondTerminal = mem_alloc(sizeof(StackItem));
        secondTerminal->Stype = S_TERMINAL;
        StackItem *nonTerminal = mem_alloc(sizeof(StackItem));
        nonTerminal->Stype = S_NONTERMINAL;
        Array_push(stack, secondTerminal);
        Array_push(stack, nonTerminal);
        *topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_TERMINAL);
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

    TEST_BEGIN("reducton of 'E!' "){
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