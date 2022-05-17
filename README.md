## MOGL
Prototype interpreter for the MOGL programming language.

## About MOGL
MOGL (McCarthy's Original Lisp) is a functional programming language inspired by the version of Lisp described in [John McCarthy's paper from 
1960](http://www-formal.stanford.edu/jmc/recursive.pdf). MOGL relies on recursion and the use of lists for problem solving.

## Grammar
```
Mogl  => M-exp*

M-exp => List | Call | NUMBER | NAME | “true” | “nil”

List  => “(” M-exp* “)”

Args  => M-exp “;” Args | M-exp

Cond  => M-exp “->” M-exp (“;” Cond)*

Call  => NAME “[” Args* “]” | "cond" "[" Cond "]"
```

## Appendix
There are more functions than those listed here, but std.mogl (our standard library) has function compositions for the missing entries.
- **nil** : value for false, terminates lists
- **true** : value for true
- **car\[x]** : returns the first element of a list
- **cdr\[x]** : returns the tail of a list
- **cons\[x;y]** : returns a list with x as the car and y as the cdr
- **(add/sub/div/mult)\[x;y]** : math operators
- **(lt/gt)\[x;y]** : less than/greater than operators
- **rem\[x;y]** : modulo operator
- **eq\[x;y]** : equality operator, only defined for atoms
- **fun\[name;args;body]** : function definition where args is a space separated list of names, and body is an m-exp (usually a cond, examples in std.mogl)
- **put\[x]** : prints the expression x
- **set\[x;y]** : binds the value of the m-exp y to the name x
- **map\[f;list]** : applies the function f to all the members of list
- **filter\[f;list]** : removes all members of the list if f\[x] returns nil
- **range\[m;n]** : returns a list containing the range m to n, inclusive

## Examples
Example MOGL code showcasing function calls, maps, filters, and ranges. ~ is our comment character
```put[fact[8]] ~ expect 40320
put[map[square;range[1;10]]] ~ expect list of numbers 1-10 squared
put[filter[even;range[1;100]]] ~ expect list of all even numbers between 1 and 100
put[map[sum; ((5 5 5) (2 2 2 2 2) (1000 1000 1000))]] ~ expect (15 10 3000)
```
## Running the interpreter
Building with `$ make` will create the executable for the interpreter. `$ ./mogl` will run the REPL, and `$ ./mogl [file]` where \[file] is the relative path 
to a .mogl file, will interpret the specified file.
