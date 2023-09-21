# Allocating memory


---


# Importing files


---


# Using `assertf`

For assertions, there is a macro `assertf` defined in `assertf.h` that works like `assert` but it's little bit smarter.

There are 3 modes:

1. `assertf(condition)`
```c
#include "assertf.h"

int num = 1;
assertf(num == 0);
```
will print:
```
[ASSERT] Assertion 'num == 0' failed
    at src/main.c:3
```

2. `assertf(condition, message)`
```c
#include "assertf.h"

int num = 1;
assertf(num == 0, "num is not equal to 0");
```
will print:
```
[ASSERT] num is not equal to 0
    at src/main.c:3
```

3. `assertf(condition, format, args...)`
```c
#include "assertf.h"

int num = 1;
assertf(num == 0, "expected num to be 0, but it is %d", num);
```
will print:
```
[ASSERT] expected num to be 0, but it is 1
    at src/main.c:3
```


---


# Printing variables

For printing variables, there is a macro `dumpvar` defined in `inspector.h` that prints content of provided variables. You can provide up to 8 variables per call. Currently supported types are `char`, `char*`, `unsigned long`, `long`, `unsigned int`, `int`, `float`, `double`, `bool`, `void*`, `String*`, `Array*`, `HashMap*` and `TextRange*`.

Example:
```c
#include "internal/String.c"
#include "inspector.h"

int num = 123;
char *chars = "Test string";
String *str = String_alloc("Hello world!");

dumpvar(num, chars, str);
```
will print:
```
from src/main.c:8:
num = 123
chars = "Test string"
str = String { "Hello World!" }
```


---


## Printing single characters

For printing a single character, instead of:
```c
printf("Unexpected character '%c' ...", my_char);
```

use

```c
#include "inspector.h"

printf("Unexpected character '%s' ...", format_char(my_char));
```

In case of some special characters, the first method can fail:

```c
char my_char = '\n';
printf("Unexpected character '%c' ...", my_char);
```

will print:

```
Unexpected character '
' ...
```

or

```c
char my_char = '\0';
printf("Unexpected character '%c' ...", my_char);
```

will print:

```
Unexpected character '
```

The second method will print:

```
Unexpected character 'LF' ...
```

and

```
Unexpected character 'NUL' ...
```

respectively.

This behaviour might be changed in the future and the final formatted strings might be different (maybe replace `NUL` with `\0`, ...?)
