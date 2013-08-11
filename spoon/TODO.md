Todo:
-----

- bulk out the standard library
- Use object destructors to give debug information as the stack is unwound: "inside while, inside function main" etc.
	- "defuse" them if the function completes successfully, otherwise they'll print an error message when destroyed.
- line numbers for syntax errors
- stack unwinding information for syntax errors ("in function main" etc.)
- have a better way of handling builtins! (some sort of map to function pointers?)
- Make code generator its own separate class, friend of linker.
- compile-to-RAM option
- export definitions and offsets of globals for RAM programs to use ROM routines.
- headers/preprocessor
- make vardict allocate from end of RAM, not start
	- otherwise we can't load programs into the start of RAM!
- make function definitions use vardeclaration::varpair instead of name and type. (lets us reuse getvarname_and_type() for array types)
- When we're doing an if:
	- if the last instruction wrote to the location that we're branching on, we've already got the ability to branch on the result.
	- so we check for this condition, and that both of the preceding skip fields point to the current index.
	- if and only if this is true, we can retroactively modify these fields, and avoid 3 instructions and an extra memory location.