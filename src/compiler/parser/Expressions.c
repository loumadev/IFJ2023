#include "compiler/parser/Parser.h"

#include <stdbool.h>

#include "assertf.h"
//#include "internal/Array.h"
//#include "compiler/parser/ASTNodes.h"
//#include "compiler/lexer/Lexer.h"
#include "compiler/parser/Expressions.h"
#include "compiler/lexer/Token.h"
#include "internal/Array.h"
#include "allocator/MemoryAllocator.c"

#define TAB_SIZE 9
#define STACK_SIZE 20

int precedence_table[TAB_SIZE][TAB_SIZE] =    //[stack top terminal][input token]
{
 // +-|*/| ! |??|r |i |( |) |$   
    {R, S, S, R, R, S, S, R, R}, // +-
    {R, R, S, R, R, S, S, R, R}, // */
    {R, R, R, R, R, S, S, R, R}, // !
    {S, S, S, S, S, S, S, R, R}, // ??
    {S, S, S, R, R, S, S, R, R}, // r (==, !=, <, >, <=, >=)
    {R, R, R, R, R, X, X, R, R}, // i
    {S, S, S, S, S, S, S, E, X}, // (
    {R, R, R, R, R, X, X, R, R}, // )
    {S, S, S, S, S, S, S, X, X}  // $

};

int Expr_getPrecTbIndex(Token *token){

    switch (token->kind){
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return I_PLUS_MINUS;
        
        case TOKEN_STAR:
        case TOKEN_SLASH:
            return I_MUL_DIV;
        
        case TOKEN_EXCLAMATION:
            return I_EXCL_OP;

        case TOKEN_NULL_COALESCING:
            return I_NIL_COALES;
        
        case TOKEN_EQUALITY:
        case TOKEN_NOT_EQUALITY:
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER_EQUAL:
            return I_REL_OP;
        
        case TOKEN_LEFT_BRACE:
            return I_LEFT_BRAC;
        
        case TOKEN_RIGHT_BRACE:
            return I_RIGHT_BRAC;      

        case TOKEN_DEFAULT:
            if(token->type == TOKEN_IDENTIFIER){
                return I_ID;
            }  //else?
        case TOKEN_STRING:
        case TOKEN_INTEGER:
        case TOKEN_FLOATING:
        case TOKEN_NIL:
            return I_ID;

        default:
            return I_DOLLAR;
    }
}

StackItem Expr_getTopTerminal(Array *stack){
    StackItem *top;
    for(int i = 0; i < stack->size; i++){
        if((top = Array_get(stack, stack->size - i))->Stype == S_TERMINAL){
            return top;
        }
    }
    // else error ?
}

bool Expr_typecheck(Array *stack){
    
}

StackItem Expr_performReduction(Array *stack){
    if (stack->size == 2){
        StackItem *operator = Array_get(stack, 1);
        switch (operator->token->kind){
            case TOKEN_PLUS:
            case TOKEN_MINUS:
            case TOKEN_STAR:
            case TOKEN_SLASH:
                if(;){//call typecheck
                    ;//call make operation
                }
                else return NULL;

            case TOKEN_EQUALITY:
            case TOKEN_NOT_EQUALITY:
            case TOKEN_LESS:
            case TOKEN_GREATER:
            case TOKEN_LESS_EQUAL:
            case TOKEN_GREATER_EQUAL:    
                if(;){//call typecheck
                    ;//call make operation
                }
                else return NULL;
            
            
            default:
                break;
        }
        
    }
}

ParserResult __Parser_parseExpression(Parser *parser) {
	assertf(parser != NULL);
    
    Array *stack = Array_alloc(STACK_SIZE);
    Array *reduceStack = Array_alloc(STACK_SIZE);
    Token *token = NULL;
    StackItem *bottom = mem_alloc(sizeof(StackItem));
    StackItem *stopReduction = mem_alloc(sizeof(StackItem));
    stopReduction->Stype = S_STOP;
    bottom->Stype = S_BOTTOM;
    Array_push(stack, bottom);

    LexerResult current = Lexer_nextToken(parser->lexer);

    while(true){

        if(!current.success) return LexerToParserError(current);
        int operation = precedence_table[Expr_getPrecTbIndex(Expr_getTopTerminal(stack).token)][Expr_getPrecTbIndex(current.token)];

        switch(operation){
            case S: 
                StackItem *token = mem_alloc(sizeof(StackItem));
                token->Stype = S_TERMINAL;
                token->token = current.token; 
                Array_push(stack, stopReduction);
                Array_push(stack, token);
                current = Lexer_nextToken(parser->lexer); //better check first with peekToken
                break;
            case R: 
                while((token = Array_pop(stack))->Stype != S_STOP){
                    if(token->kind != S_STOP){
                        Array_push(reduceStack, token);
                    }
                }
                //perform reduction
                //push result on stack (nonterminal)
                break;
            case E: 
                StackItem *token = mem_alloc(sizeof(StackItem));
                token->Stype = S_TERMINAL;
                token->token = current.token;
                Array_push(stack, token);
                break;
            case X:
                //call error 
                break;
            default:
                break;


        }
    }
	
	return ParserNoMatch();
}