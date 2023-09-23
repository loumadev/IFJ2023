# Memory management

The project contains a custom memory allocator with "garbage collection". That means that you don't have to free memory manually, but you can do it if you want to.

**!!!** Please avoid using default `malloc` and `free` functions, because they don't work with the custom allocator. Custom allocator can be explicitly set to use default memory allocator, but it's not recommended.

To use custom memory allocator, please include `allocator/MemoryAllocator.c` to your project file.

All memory functions are safe, thus they never return `NULL` and handle the errors internally, so you don't have to worry about it.

**Note:** To force use default memory allocator, use `safe_malloc`, `safe_calloc`, `safe_realloc` and `safe_free` functions. Be aware that memory allocated by these functions has to be freed manually and causes memory leaks if not freed properly! (This is usually not needed by the end user; it is mainly used internally, but the API is exposed.)

## void* mem_alloc(size_t size)

Similar to `malloc`, but it never returns `NULL`. Memory allocated by this function is automatically freed when the program ends.

```c
char *str = mem_alloc(sizeof(char) * 10);
...
mem_free(str); // free memory explicitly
```

## void* mem_calloc(size_t num, size_t size)

Similar to `calloc`, but it never returns `NULL`. Memory allocated by this function is automatically freed when the program ends.

```c
char *str = mem_calloc(10, sizeof(char));
...
mem_free(str); // free memory explicitly
```

## void* mem_realloc(void *ptr, size_t size)

Similar to `realloc`, but it never returns `NULL`. Memory allocated by this function is automatically freed when the program ends.

```c
char *str = mem_alloc(sizeof(char) * 10);
...
str = mem_realloc(str, sizeof(char) * 20);
...
mem_free(str); // free memory explicitly
```

## void mem_free(void *ptr)

Explicitly frees memory allocated by `mem_alloc`, `mem_calloc` or `mem_realloc` functions.

```c
char *str = mem_alloc(sizeof(char) * 10);
...
mem_free(str); // free memory explicitly
```


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
