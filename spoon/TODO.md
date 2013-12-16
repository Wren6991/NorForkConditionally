Todo:
-----

- bulk out the standard library
- Use object destructors to give debug information as the stack is unwound: "inside while, inside function main" etc.
	- "defuse" them if the function completes successfully, otherwise they'll print an error message when destroyed.
- line numbers for syntax errors
- stack unwinding information for syntax errors ("in function main" etc.)
- Separate memory spaces (compile to RAM option)
	- no need to link const table, add pointer runtime support if in a RAM program
- fix that x = const int bug
- Make macros sane (+ possibly ruby yield?)
- When we're doing an if:
	- if the last instruction wrote to the location that we're branching on, we've already got the ability to branch on the result.
	- so we check for this condition, and that both of the preceding skip fields point to the current index.
	- if and only if this is true, we can retroactively modify these fields, and avoid 3 instructions and an extra memory location.