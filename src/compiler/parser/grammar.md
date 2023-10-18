# Grammar

IFJ23

## base

assignment-expression → `=` expression<br>


type-annotation → `:` type<br>
type → `String` | `Int` | `Double`<br>

code-block → `{`statements?`}`<br>

## expressions

expression → TODO

## statements

statement → function-declaration<br>
statement → function-call<br>
statement → variable-declaration<br>
statement → variable-assignmen<br>
statement → if-statement<br>
statement → while-statement<br>
statement → return-statement<br>
statement → expression<br>

## function declaration

function-declaration → function-head function-name function-signature function-body?<br>

function-head → func<br>
function-name → identifier<br>

function-signature → parameter-clause function-result?<br>
function-result → `->` type<br>
function-body → code-block<br>

parameter-clause → `(` `)` | `(` parameter-list `)`<br>
parameter-list → parameter | parameter `,` parameter-list<br>
parameter → external-parameter-name? local-parameter-name type-annotation<br>
external-parameter-name → identifier<br>
local-parameter-name → identifier<br>

### ? not in the project specification ?<br>
parameter → external-parameter-name? local-parameter-name type-annotation default-argument-clause?<br>
default-argument-clause → `=` expression<br>

## function call

argument-name identifier<br>
argument-signature<br>

argument → argument-name`:` expression<br>
argument → expression<br>

argument-list → argument | argument , argument-list<br>
argument-clause → `(` `)` | `(` argument-list `)`<br>

function-call → function-name argument-clause<br>

## function call (assignment) ?expression?
TODO

id = název_funkce(seznam_vstupních_parametrů)<br>
let id = název_funkce(seznam_vstupních_parametrů)<br>

## variable declaration
variable-declaration → variable-head variable-name type-annotation assignment-expression?<br>
variable-head → let | var<br>
variable-name → identifier<br>

## variable assignment
 variable-assignment → variable-name assignment-expression?<br>
 variable-name → identifier<br>

## if statement

condition → expression<br>
condition → let variable-name<br>

if-statement → if condition code-block else-clause<br>
else-clause → else code-block<br>

## while statement

while-statement → while condition code-block<br>

## return statement

return-statement → return expression?<br>
