# CLOX
[https://craftinginterpreters.com/](https://craftinginterpreters.com/)

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
The VM contains a reference to a chunk (why not own it?), as well as the `ip` (instruction pointer) which indicates which instruction it's currently executing.