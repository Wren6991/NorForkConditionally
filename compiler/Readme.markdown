Language Description: Fork
==========================

Fork is essentially an assembly language, with a nice macro system and facilities for variable declaration and management.
A fork command consists of 2 or 4 literal hex integers:

`c000 c001 0008 0008`

These correspond to the NFC address fields ABCD. Each field is a 16 bit integer; They are _always_ hex, never decimal. The above command NORs `c000` (the debug output port) with the value of the debug input port, and then skips to address `0008` irrespective of the result.
If the command is only 2 fields long, the address of the next instruction is automatically "stitched in" so that instructions are executed sequentially. E.g.:

`c000 c001`

is equivalent to the above, assuming the code starts at `0000`.

Sections Covered:

- Macros
    - Value macros
    - Function macros
- Variables
    - Var statement
    - Free Statement
- Labels
- Comments
- Literal Values
- Dat Statements
- Buffer Statements
- Examples

Macros
------

Obviously the above is quite a painful way to program - to that end there is a macro system, designed to make the language slightly more expressive.
There are two types of macro, value and function.

**Value Macros**

Value macros allow you to define a constant: all instances of this constant are replaced with the specified value when the code is compiled. 
They are written in the form `def name value`. For example:
```
def debugout c000
def debugin c001
debugout debugin
```
The last line is now the same as the first shown command, `c000 c001` - however, it is much more clear as to its significance.
Do keep in mind that command arguments are addresses, not values - so the below:
```
def one 1
debugout one
```
will NOR with the value at the _address_ one, which is probably not what you intended! Literal values are included for this purpose, read on below.

**Function Macros**

A function macro takes arguments, and expands to a series of commands - they can include other macros, but recursive macros are likely to put the compiler in an infinite loop. _(I haven't tried this though.)_

A function macro definition is of the following form:

```
def macro(arg1, arg2, arg3)
    command1
    command2
end
```

and called as such:

`macro(x, y, z)`

This call will expand into the given definition at compile time, with the arguments replaced textually. Macros are used rather than true functions because of their ease of execution - inserting code is much simpler and faster than a call routine, although more space is used. Macros do _not_ have return values - they are commands.
Here are some genuine macro examples:

```
def clear(address)
    address 'ff
end

def invert(address)
    address address
end
```

Note the `'ff` literal in the clear macro - this is linked in from a constant table, described further down the page.

Variables
---------

**Var Statement**

If you need a piece of memory so that you can store a temporary value, you can use a variable. Variables are declared with a `var` statement:

`var a, b, c`

This allocates 3 variables - `a`, `b`, and `c` now refer to their locations. The compiler links each variable name to a free byte at the end of RAM.
Each variable is one byte in size - if you need a larger chunk of data then you should pick your own memory addresses in lower RAM, as the compiler does not guarantee the adjacency of the variables.

**Free Statement**

When you are finished with a variable, free the memory with the `free` statement:

`free a, b, c`

This tells the compiler that you are no longer using that specific memory location, which frees it up for other variables.
Here is an example of a macro that uses a variable:

```
def jump(address)
    var x
    x x address address
    free x
end
```

A variable x is allocated as a piece of scratch space for a jump instruction - we don't care about its initial or final value, but we know we are writing to an unused location so there won't be any side effects.
Note that variable allocation and linking is all done at compile time - these commands do not affect the runtime speed of your Fork code.

Labels
------

The NFC instruction allows you to jump or fork to an address, but it does not make clear the significance of this address - worse still, the address to be jumped to will _change_ if code is added or removed.

To avoid this problem, you can use a label:

```
loopStart:
    invert(debugout)
    jump(loopStart)
```
A label consists of an identifier followed by a colon, at the beginning of a line. It does not have to be on its own line. The above code will invert the debug output value and then jump back to the first instruction, in a never-ending loop.

Comments
--------

It's generally considered good practice to document your code, describing what it does (and _why_) - in Fork, you can add a comment using a `#` symbol.

```
# Set (address) to 0:
def clear(address)
    address 'ff      # not(a or 0xff) is always equal to 0.
end
```

Comments are ignored by the compiler, but this does not mean they are useless:: when not used excessively, comments make code much easier to understand.

Literal Values
--------------

The arguments of the NFC instruction are all _address_ arguments - they are a location of a value, not an actual value. If you want to pass a literal value to a macro or instruction, you must use a literal form, i.e.`'xx`: this is a literal 8-bit hex value.
These values are all stored in ROM immediately after your compiled code, and your literal form in code is replaced with the address of this value, meaning that code using the literal will load the correct value.

Dat Statements
--------------

If you want to include a sequence of values in your code, you should use a dat statement - this is a series of n-byte values that gets assembled straight into your compiled code. In order to link to this data, you should put a label on or immediately above the dat statement:

```
string:
dat 48 65 6c 6c 6f 31		# "Hello!" in ASCII.
```

Buffer Statements
-----------------

Sometimes, you want your code (or data) to line up with a certain memory offset - for instance, when implementing lookup tables.
To this end, Fork provides a buffer statement:

`buffer 4000`

The first command after this statement will be at offset 0x4000, or at the 16 KiB mark.
If you try to buffer to a location that you are already past, you will be notified and the code will fail to compile.

Examples
--------
