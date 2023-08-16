# Stack-based if/else/endif handling

Simple stack-based implementation of handling `if`-[`else`]-`endif` statements
while parsing a file line-by-line, such as the C preprocessor.

This is just some code to play around with an "if stack" and easily test the
code, without having to build a larger project for which this might be used.

## Building

Just run `make`.

Please note the code uses a few POSIX functions such as `strcasecmp(3)` in the
test driver file `main.c`, so it isn't portable, but good enough for my use case.
The if-stack code itself (`ifstack.c`, `ifstack.h` is fully C99-compliant).

## API

### Initialization and cleanup

Initialize the stack with `ifstack_init()`, free after use with `ifstack_free()`.
To use the stack again to parse another file, use `ifstack_reset()`, which
essentially calls `ifstack_free()` followed by `ifstack_init()`.

### Handling if, else and endif

Three functions are available to handle **if**, **else** and **endif**:

```c
void ifstack_if(bool state);
bool ifstack_else(bool state);
bool ifstack_endif(void);
```

When encountering an **if** the parser should call `ifstack_if()` with the
boolean condition parsed from the statement's argument.

When encountering an **else** or an **endif** the parser calls `ifstack_else()`
and `ifstack_endif()` respectively.

The global truth condition is checked with `bool ifstack_true(void)`, the parser
should still register any **if**, **else** and **endif** it encounters if the
global condition is `false`, so the if-stack can properly detect which **endif**
closes which **if**/**else** branch.

### Error reporting

```c
extern int  ifstack_errno;
const char *ifstack_strerror(int errnum);
```

The global variable `int ifstack_errno` contains the error number, should any
function return `false` to indicate an error. The message for the number can be
obtained with `ifstack_sterror()`.

