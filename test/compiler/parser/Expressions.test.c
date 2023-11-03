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
    int num = 123;
    dumpvar(num);
    Array_push(stack, dollarTerminal);
    dumpvar(stack);
    TEST_BEGIN("one item on stack"){
        *topTerminal = Expr_getTopTerminal(stack);
        EXPECT_TRUE(topTerminal->Stype == S_BOTTOM);
    }TEST_END();



}