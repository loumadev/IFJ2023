## LL-Gramatika
1. program → statements
2. program → ε
3. code-block → `{` statements `}`
4. statements → statement statements
5. statements → ε
6. statement → function-declaration
7. statement → variable-declaration
8. statement → if-statement
9. statement → while-statement
10. statement → for-in-statement
11. statement → continue-statement
12. statement → break-statement
13. statement → return-statement
14. statement → function-call-expression
15. statement → expression-statement
16. expression-statement → expression
17. function-declaration → `func` function-name function-signature function-body
18. function-name → identifier
19. function-signature → parameter-clause function-result
20. function-result → `->` type
21. function-result → ε
22. function-body → code-block
23. function-body → ε
24. parameter-clause → `(` parameter-list `)`
25. parameter-list → parameter
26. parameter-list → parameter `,` parameter-list
27. parameter-list → ε
28. parameter → external-parameter-name local-parameter-name type-annotation initializer
29. external-parameter-name → identifier
30. local-parameter-name → identifier
31. initializer → `=` expression
32. initializer → ε
33. function-call-expression → function-name argument-clause
34. argument-clause → `(` argument-list `)`
35. argument-list → argument 
36. argument-list → argument `,` argument-list
37. argument-list → ε
38. argument → argument-name `:` expression
39. argument → expression
40. argument-name → identifier
41. assignment-statement → variable-name assignment-expression
42. assignment-expression → `=` expression
43. variable-declaration → variable-head variable-declaration-list
44. variable-head → `let` 
45. variable-head → `var`
46. variable-name → identifier
47. variable-declaration-list → variable-declarator 
48. variable-declaration-list → variable-declarator `,` variable-declaration-list
49. variable-declaration-list → ε
50. variable-declarator → pattern initializer
51. pattern → variable-name type-annotation
52. type-annotation → `:` type
53. type-annotation → ε
54. type → identifier
55. if-statement → `if` condition code-block else-clause
56. else-clause → `else` code-block 
57. else-clause → `else` if-statement
58. else-clause → ε
59. condition → expression 
60. condition → optional-binding-condition
61. optional-binding-condition → `let` pattern initializer 
62. optional-binding-condition → `var` pattern initializer
63. while-statement → `while` condition code-block
64. for-in-statement → `for` variable-name `in` range code-block
65. range → close-range 
66. range → half-open-range
67. close-range → expression...expression
68. half-open-range → expression..<expression
69. continue-statement → `continue`
70. break-statement → `break`
71. return-statement → `return` expression
72. return-statement → `return`
