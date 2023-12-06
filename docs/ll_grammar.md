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
14. statement → expression-statement
15. expression-statement → function-call-expression
16. function-declaration → `func` function-name function-signature function-body
17. function-name → identifier
18. function-signature → parameter-clause function-result
19. function-result → `->` type
20. function-result → ε
21. function-body → code-block
22. function-body → ε
23. parameter-clause → `(` parameter-list `)`
24. parameter-list → parameter
25. parameter-list → parameter `,` parameter-list
26. parameter-list → ε
27. parameter → external-parameter-name local-parameter-name type-annotation
28. external-parameter-name → identifier
29. local-parameter-name → identifier
30. function-call-expression → function-name argument-clause
31. argument-clause → `(` argument-list `)`
32. argument-list → argument
33. argument-list → argument `,` argument-list
34. argument-list → ε
35. argument → argument-name `:` expression
36. argument → expression
37. argument-name → identifier
38. assignment-statement → variable-name `=` expression
39. variable-declaration → variable-head variable-declaration-list
40. variable-head → `let`
41. variable-head → `var`
42. variable-name → identifier
43. variable-declaration-list → variable-declarator
44. variable-declaration-list → variable-declarator `,` variable-declaration-list
45. variable-declaration-list → ε
46. variable-declarator → pattern `=` expression
47. variable-declarator → pattern
48. pattern → variable-name type-annotation
49. type-annotation → `:` type
50. type-annotation → ε
51. type → identifier
52. if-statement → `if` condition code-block else-clause
53. else-clause → `else` code-block
54. else-clause → `else` if-statement
55. else-clause → ε
56. condition → expression
57. condition → optional-binding-condition
58. optional-binding-condition → variable-head variable-declarator
59. while-statement → `while` condition code-block
60. for-in-statement → `for` variable-name `in` range code-block
61. range → expression range-operator expression
62. range-operator → `...`
63. range-operator → `..<`
64. continue-statement → `continue`
65. break-statement → `break`
66. return-statement → `return` expression
67. return-statement → `return`
