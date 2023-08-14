# Stack-based if/else/endif handling

Simple stack-based implementation of handling `if`-[`else`]-`endif` statements
while parsing a file line-by-line, such as the C preprocessor.

This is just some code to play around with an "if stack" and easily test the
code, without having to build a larger project for which this might be used.

## Building

Just run `make`.

Please note the code uses a few POSIX functions such as `strcasecmp(3)`, so it
isn't portable, but good enough for my use case.
