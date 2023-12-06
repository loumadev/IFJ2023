# Grammar

IFJ23

## base

program → statements?
code-block → `{` statements? `}`<br>

statements → statement statements

type-annotation → `:` type<br>
type → identifier

## literals

literal → numeric-literal | string-literal | boolean-literal | nil-literal<br>

? numeric-literal → `-`? integer-literal | `-`? floating-point-literal // Should this be included? Number literals are defined by FSM diagram<br>
boolean-literal → `true` | `false`<br>
nil-literal → `nil`<br>

## expressions

expression → TODO<br>

// This is probably wrong (swift parses `a = b` as a `BinaryExpression`, but we want to be more explic, so this would be parsed as `AssignmentExpression`), so it needs rework; it's just to show the `function-call-expression` rule (this way the function call can appear anywhere in the expression (`myFunc(otherFunc(10))`, `myFunc() + otherFunc(10) * 8`, `a = myFunc() * -1`, ...))

expression → prefix-expression infix-expressions?<br>
expression-list → expression | expression `,` expression-list<br>

prefix-expression → prefix-operator? postfix-expression<br>

infix-expression → infix-operator prefix-expression<br>
infix-expression → assignment-operator prefix-expression<br>
infix-expression → conditional-operator prefix-expression<br>
? infix-expression → type-casting-operator // Are we gonna support explicit type casting?<br>
infix-expressions → infix-expression infix-expressions?<br>

postfix-expression → primary-expression<br>
postfix-expression → postfix-expression postfix-operator<br>
postfix-expression → function-call-expression<br>

primary-expression → identifier<br>
primary-expression → literal-expression<br>
primary-expression → parenthesized-expression<br>

assignment-operator → `=`<br>

assignment-expression → `=` expression<br>

conditional-operator → `?` expression `:`<br>

parenthesized-expression → `(` expression `)`<br>

literal-expression → literal<br>

## statements

statement → function-declaration<br>
statement → variable-declaration<br>
statement → if-statement<br>
statement → while-statement<br>
statement → for-in-statement<br>
statement → continue-statement<br>
statement → break-statement<br>
statement → return-statement<br>
statement → expression-statement<br>

expression-statement → expression<br>

## function declaration

function-declaration → `func` function-name function-signature function-body?<br>

function-name → identifier<br>

function-signature → parameter-clause function-result?<br>
function-result → `->` type<br>
function-body → code-block<br>

parameter-clause → `(` `)` | `(` parameter-list `)`<br>
parameter-list → parameter | parameter `,` parameter-list<br>
parameter → external-parameter-name local-parameter-name type-annotation initializer?<br>
external-parameter-name → identifier<br>
local-parameter-name → identifier<br>

initializer → `=` expression<br>

## function call

function-call-expression → function-name argument-clause<br>

argument-clause → `(` `)` | `(` argument-list `)`<br>
argument-list → argument | argument `,` argument-list<br>

argument → argument-name `:` expression<br>
argument → expression<br>

argument-name → identifier<br>

## assigment statement

assignment-statement → variable-name assignment-expression<br>

## variable declaration

variable-declaration → variable-head variable-declaration-list?<br>
variable-head → `let` | `var`<br>
variable-name → identifier<br>

variable-declaration-list → variable-declarator | variable-declarator `,` variable-declaration-list<br>
variable-declarator → pattern initializer?<br>
pattern → variable-name type-annotation?<br>

## if statement

condition → expression | optional-binding-condition<br>
optional-binding-condition → `let` pattern initializer? | `var` pattern initializer?

if-statement → `if` condition code-block else-clause?<br>
else-clause → `else` code-block | `else` if-statement<br>

## while statement

while-statement → `while` condition code-block<br>

## for in statement

for-in-statement → `for` variable-name `in` range code-block<br>

range → close-range | half-open-range<br>
close-range → expression...expression<br>
half-open-range → expression..<expression<br>

## continue statement

continue-statement → `continue`<br>

## break statement

break-statement → `break`<br>

## return statement

return-statement → `return` expression?<br>
