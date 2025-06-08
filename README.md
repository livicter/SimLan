# This is a toy language!

## Overall Compiler Flow

When you run simlanc demo.simlan, this is what happens at a high level:

### main.cpp:
Reads the content of demo.simlan into a string.
Creates a Lexer object, passing it the source code.
Creates a Parser object, passing it the Lexer.
Calls the Parser's main method (parseProgram()) to build an Abstract Syntax Tree (AST).
If parsing is successful, it first calls the print() method on the AST's root node to display its structure.
Then, it calls the execute() method on the AST's root node to interpret the program and produce the output.

## This can be visualized as:
[demo.simlan (text)] --> Lexer --> [Tokens] --> Parser --> [AST] --> Interpreter (execute methods) --> [Program Output]

## Compilation and Run
user:/build$ make
[ 20%] Building CXX object CMakeFiles/simlanc.dir/src/main.cpp.o
[ 40%] Building CXX object CMakeFiles/simlanc.dir/src/lexer.cpp.o
[ 60%] Building CXX object CMakeFiles/simlanc.dir/src/parser.cpp.o
[ 80%] Building CXX object CMakeFiles/simlanc.dir/src/ast.cpp.o
[100%] Linking CXX executable simlanc
[100%] Built target simlanc
user:/build$ ./simlanc ../demo.simlan
