## LL-Gramatika

1. program → statements
2. code-block → `{` statements `}`
3. statements → statement statements
4. statements → ε
5. statement → function-declaration
6. statement → variable-declaration
7. statement → if-statement
8. statement → while-statement
9. statement → for-in-statement
10. statement → continue-statement
11. statement → break-statement
12. statement → return-statement
13. statement → expression-statement
14. expression-statement → function-call-expression
15. function-declaration → `func` function-name function-signature function-body
16. function-name → identifier
17. function-signature → parameter-clause function-result
18. function-result → `->` type
19. function-result → ε
20. function-body → code-block
21. function-body → ε
22. parameter-clause → `(` parameter-list `)`
23. parameter-list → parameter additional-parameters
24. parameter-list → ε
25. additional-parameters → `,` parameter additional-parameters
26. additional-parameters → ε
27. parameter → external-parameter-name local-parameter-name type-annotation
28. external-parameter-name → identifier
29. local-parameter-name → identifier
30. function-call-expression → function-name argument-clause
31. argument-clause → `(` argument-list `)`
32. argument-list → argument additional-arguments
33. argument-list → ε
34. additional-arguments → `,` argument additional-arguments
35. additional-arguments → ε
36. argument → argument-name `:` expression
37. argument → expression
38. argument-name → identifier
39. assignment-statement → variable-name `=` expression
40. variable-declaration → variable-head variable-declaration-list
41. variable-head → `let`
42. variable-head → `var`
43. variable-name → identifier
44. variable-declaration-list → variable-declarator additional-declarators
45. additional-declarators → `,` variable-declarator additional-declarators
46. additional-declarators → ε
47. variable-declarator → pattern initializer
48. initializer → `=` expression
49. initializer → ε
50. pattern → variable-name type-annotation
51. type-annotation → `:` type
52. type-annotation → ε
53. type → identifier
54. if-statement → `if` condition code-block else-clause
55. else-clause → `else` else-body
56. else-clause → ε
57. else-body → if-statement
58. else-body → code-block
59. condition → expression
60. condition → optional-binding-condition
61. optional-binding-condition → variable-head variable-declarator
62. while-statement → `while` condition code-block
63. for-in-statement → `for` variable-name `in` range code-block
64. range → expression range-operator expression
65. range-operator → `...`
66. range-operator → `..<`
67. continue-statement → `continue`
68. break-statement → `break`
69. return-statement → `return` expression
