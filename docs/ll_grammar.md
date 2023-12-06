## LL-Gramatika
1. program → statements
2. program → ε
3. code-block → `{` statements `}`
4. statements → statement statements
5. statements → ε
8. statement → function-declaration
9. statement → variable-declaration
10. statement → if-statement
11. statement → while-statement
12. statement → for-in-statement
13. statement → continue-statement
14. statement → break-statement
15. statement → return-statement
16. statement → expression-statement
17. expression-statement → expression
18. function-declaration → `func` function-name function-signature function-body
19. function-name → identifier
20. function-signature → parameter-clause function-result
21. function-result → `->` type
22. function-result → ε
23. function-body → code-block
24. function-body → ε
25. parameter-clause → `(` parameter-list `)`
26. parameter-list → parameter
27. parameter-list → parameter `,` parameter-list
28. parameter-list → ε
29. parameter → external-parameter-name local-parameter-name type-annotation initializer
30. external-parameter-name → identifier
31. local-parameter-name → identifier
32. initializer → `=` expression
33. initializer → ε
34. function-call-expression → function-name argument-clause
35. argument-clause → `(` argument-list `)`
36. argument-list → argument 
37. argument-list → argument `,` argument-list
38. argument-list → ε
39. argument → argument-name `:` expression
40. argument → expression
41. argument-name → identifier
42. assignment-statement → variable-name assignment-expression
43. assignment-expression → `=` expression
44. variable-declaration → variable-head variable-declaration-list
45. variable-head → `let` 
46. variable-head → `var`
47. variable-name → identifier
48. variable-declaration-list → variable-declarator 
49. variable-declaration-list → variable-declarator `,` variable-declaration-list
50. variable-declaration-list → ε
51. variable-declarator → pattern initializer
52. pattern → variable-name type-annotation
53. type-annotation → `:` type
54. type-annotation → ε
55. type → identifier
56. if-statement → `if` condition code-block else-clause
57. else-clause → `else` code-block 
58. else-clause → `else` if-statement
59. else-clause → ε
60. condition → expression 
61. condition → optional-binding-condition
62. optional-binding-condition → `let` pattern initializer 
63. optional-binding-condition → `var` pattern initializer
64. while-statement → `while` condition code-block
65. for-in-statement → `for` variable-name `in` range code-block
66. range → close-range 
67. range → half-open-range
68. close-range → expression...expression
69. half-open-range → expression..<expression
70. continue-statement → `continue`
71. break-statement → `break`
72. return-statement → `return` expression
73. return-statement → `return`
