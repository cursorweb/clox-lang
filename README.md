# CLOX
[https://craftinginterpreters.com/](https://craftinginterpreters.com/)

## Building
Use the *x86 native tools command prompt* to open vscode.
Then, run `./build.ps1`
You can clean build files using `./clean.ps1`

## Frontend Backend
**Frontend** Compiler
**Representation** Bytecode
**Execution** VM

~ Parser, AST, Interpreter

## Chunks and stuff
`chunk.h` A *chunk* is a chunk of bytecode ~ AST. It contains:
* a vector that contains instructions aka code aka opcode
* a vector of its corresponding line info (one line per each code).
* the *valueArray*.


`value.h` A *valueArray* is a vector that contains all the constants that the program uses, for example
```js
var a = 5;
```
`5` will be added to the value array.

To look up the value array, the bytecode looks like:
```
OP_CONSTANT
0
```
The second byte is the index of the valueArray where the constant is stored.

`vm.h`
The VM contains a reference to a chunk (so it doesn't copy after compilation), as well as the `ip` (instruction pointer) which indicates which instruction it's currently executing.

## Compiler
Rather than parsing to produce an AST and then turning it into bytecode, the compiler is going to do the two in the same pass.

## Reading
[Dragon Book](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools)
[Trie](https://en.wikipedia.org/wiki/Trie)
[Finite State Machine DFA](https://en.wikipedia.org/wiki/Deterministic_finite_automaton)
[String Interpolation](https://github.com/wren-lang/wren/blob/8fae8e4f1e490888e2cc9b2ea6b8e0d0ff9dd60f/src/vm/wren_compiler.c#L118-L130)
[Pratt Parser](https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html)
[Pratt Parser (munificent)](https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/)

## New Features
* Format Strings
* Power (**Added**)
* Ternary Operator
* Arrays
* Weird: Option to REMOVE TYPE CHECKING and be SUPER UNSAFE for fast
* `a?` short circuit if a is null
* `a <=> b`
* `a++`; `a += 5`
* `import "file_path"`
* `import stdlib;`:
    * `import math;`