# Grammar

IFJ23

## base

assignment-expression → `=` expression

type-annotation → `:` type
type → `String` | `Int` | `Double`

code-block → `{`statements?`}`

## expressions

expression → TODO

## statements

statement → function-declaration
statement → function-call
statement → variable-declaration
statement → variable-assignment
statement → if-statement
statement → while-statement
statement → return-statement
statement → expression

## function declaration

function-declaration → function-head function-name function-signature function-body?

function-head → func
function-name → identifier

function-signature → parameter-clause function-result?
function-result → `->` type
function-body → code-block

parameter-clause → `(` `)` | `(` parameter-list `)`
parameter-list → parameter | parameter `,` parameter-list
parameter → external-parameter-name? local-parameter-name type-annotation
external-parameter-name → identifier
local-parameter-name → identifier

### ? not in the project specification ?
parameter → external-parameter-name? local-parameter-name type-annotation default-argument-clause?
default-argument-clause → `=` expression

## function call

argument-name identifier
argument-signature

argument → argument-name`:` expression
argument → expression

argument-list → argument | argument , argument-list
argument-clause → `(` `)` | `(` argument-list `)`

function-call → function-name argument-clause

## function call (assignment) ?expression?
TODO

id = název_funkce(seznam_vstupních_parametrů)
let id = název_funkce(seznam_vstupních_parametrů)

## variable declaration
variable-declaration → variable-head variable-name type-annotation assignment-expression?
variable-head → let | var
variable-name → identifier

## variable assignment
 variable-assignment → variable-name assignment-expression?
 variable-name → identifier

## if statement

condition → expression
condition → let variable-name

if-statement → if condition code-block else-clause
else-clause → else code-block

## while statement

while-statement → while condition code-block

## return statement

return-statement → return expression?
