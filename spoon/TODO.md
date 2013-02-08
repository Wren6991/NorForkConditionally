Todo:
-----

- Optimize "emit_branchalways" to modify the skip fields of the previous instruction where possible
	- 0-byte jump instructions!
- Create "object" class containing the file's definitions
	- make it (de)serializable?
- bulk out the standard library
- add comments to tokenizer!
- functions in expressions (with results)
	- have a proper return mechanism for user-defined etc.
	- have hardcoded ones like const(8), second(16), first(16) etc.
	- means we need a proper function signature class for return types
- if we assign from a function result, pass the assignment target into the function link routine.
	- this way we can write straight to the assignment target
	- perhaps save this for after everything's working?
	- still probably better for function return location to be decided for caller, more convenient that way
- Use object destructors to give debug information as the stack is unwound: "inside while, inside function main" etc.
	- "defuse" them if the function completes successfully, otherwise they'll print an error message when destroyed.
- Clear up distinctions between what's a value and what's a pointer to a value.
- make linker valtable contain linkvals, so we can have chains of symbols (sym1->sym2->constant) (is this necessary?)
- return statements in functions
	- the block and statement compile funcs will need to know the current function name
- break/continue
	- have labels, pass them into each block at link time, and jump to them on break/continue statements.

