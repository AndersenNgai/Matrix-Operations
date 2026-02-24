# Matrix Operations

This program runs in the command line. To use it, you must compile it using the GCC compiler `gcc src/* -I include -o text_replacer`.

# How to Use

In the command line, execute `text_replacer` followed 2 additional arguments - an input and output file. An input file may look like this:

DEF A 2 3 [1, 2, 3; 4, 5, 6]
DEF B 2 3 [7, 8, 9; 10, 11, 12]
DEF C 3 1 [100; 101; 102;]
EVAL D ADD A B
EVAL E SUB A B
EVAL F MUL A C
PRINT D
PRINT E
PRINT F

Each instruction must begin with `DEF`, `EVAL` or `PRINT`. `DEF` defines a new matrix from given numbers, `EVAL` defines a new matrix from an operation and `PRINT` prints a matrix to the output file.
