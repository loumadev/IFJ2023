# Grammar of a function declaration

function-declaration → function-head function-name function-signature function-body?

function-head → func
function-name → identifier

function-signature → parameter-clause function-result?
function-result → -> type
function-body → code-block

parameter-clause → ( ) | ( parameter-list )
parameter-list → parameter | parameter , parameter-list
parameter → external-parameter-name? local-parameter-name type-annotation default-argument-clause?
parameter → external-parameter-name? local-parameter-name type-annotation
external-parameter-name → identifier
local-parameter-name → identifier
default-argument-clause → = expression
