<h2>
    <a name="_Toc376623627">Language Specification</a>
</h2>
*Disclaimer: This is ripped straight from my A level writeup (a Word document), so some of the formatting's probably messed up and the appendices don't exist. I'll sort this out!*


    The language is called Spoon. (As the successor to Fork, this was the only logical choice, besides “Knife”. Knife would be banned in schools anyway, unless
    I rounded the ends and made it out of plastic.)

    The syntax of the language is defined by the following BNF/regular expressions:
```
<program> ::= {<constdef>|<funcdef>|<macrodef>|<vardecl>}

<constdef> ::= `const` <type> <name> `=` <expression> `;`

<funcdef> ::= `function` [<type>] <name> `(` <type> <name> {`,` <type> <name>} `)` <block>

<type> ::= `int`|`pointer`|`void`

<macrodef> ::= `macro` <name> `(` <name> {`,` <name>} `)` <block>

<block> ::=  <statement>
         |   `{` {<vardecl>} {<statement>} `}`

<vardecl> ::= <type> <varname> {`,` <varname>} `;`

<varname> ::= <name> [`[` <number> `]`] [`=` <expression>]

<statement> ::=  <name> `(` <expression> {`,` <expression>} `)` `;`
             |   <name> `=` <expression> `;`
             |   `if` `(` <expression> `)` <block> [`else` <block>]
             |   `while` `(` <expression> `)` <block>
             |   <name> `:`
             |   `goto` <name> `;`
             |   (`break` | `continue` | `return`) `;`
           


<expression> ::= <value> [(`&&` | `||`) <expression>]

<value> ::= `!`<value>
         |  `(`<expression>`)`
         |  <name>
         |  <name> `(`<expression> {`,` <expression>}`)`
         |  <name> `[`<number>`]`
         |  <string>
         |  `{`<number> {`,` <number>} `}`
         |  <number>

<name> ::= [a-zA-Z_][a-zA-Z0-9_]*

<number> ::= (0x[0-9a-fA-F]+)|([0-9]+)
          |  `'`<char>`'`

<string> ::= \"[^\"]*\"
```
Where non-terminals are in &lt;angle-brackets&gt; , terminals are in `backquotes`, `[x]` represents optional `x` and `{x}` represents optional multiple `x` (a Kleene closure).


Identifiers and operators are all case sensitive.

A program is a set of declarations: of functions, of macros, of variables and of constants. The only stipulation is that there must exist a function called main, which the linker will use as the program entry point. main takes no arguments and returns `void`.

<h3>
    <a name="_Toc376623628">Types</a>
</h3>

There are 3 main types: `void`, `int` and `pointer`. You can also use the name char instead of `int`, or `int16` instead of `pointer`, but they are logically and internally the same.

`void` is a pseudo-type, and represents the result of a function that does not return anything. You can define a variable of type `void`, but trying to assign anything but the result of a `void` function to it will generate a type error. (Why you would want to do this is beyond me, but it is logically consistent).It has a size of 0 bytes.

`int` has a size of one byte (8 bits). It is the basic unit of arithmetic, and can also represent a single ASCII character. The nickname char is a completely equivalent type, but is a little more natural when writing text processing code (a la C).

`pointer` is 2 bytes long, or 16 bits. It refers to a single 8-bit word of machine memory, or can be used to represent a large integer, such as the size of a file in bytes (hence the pseudonym `int16`).

The only compound type is the array. If you need to represent a complex data structure (like C’s struct), use an array.

<h3>
    <a name="_Toc376623629">Variable Declarations and Scope</a>
</h3>

Variable declarations can take place anywhere inside a block, but scoping is at a block-level – this has the effect that all variable declarations are processed upon entering a block.


Multiple variables of the same type can be declared at once, and each variable can be initialised with any valid expression:

```       
var `int` x = 5, y = f(x);
```

The initialisations are guaranteed to be performed left-to-right, so f will receive an argument of 5 in the above example.

While the declarations are processed as though they were at the top of a block, the initializations are compiled in the position they are written, so there is no loss of sequencing.

Variables are lexically scoped, as per Algol, C, Haskell and Common Lisp. For example:
```
function main()
{
    var int a, b, c;
    a = 5;
    if (true)
    {
        var int a;
        a = increment(a);
    }
    output(a, debugout);
}
```
Will result in an output of 5, because the new a defined inside the if block “shadows” the old one, so the original a is unchanged.

<h3>
    <a name="_Toc376623630">Arrays</a>
</h3>

    The language allows you to define arrays:
```                
var int arr[5];
```
An area of size n * sizeof(type) bytes is allocated by the linker and guaranteed not to be touched by other variables for the duration of the current
block, e.g. an array of 5 `pointer`s would be 10 bytes. You can assign `int`o arrays if you have a constant offset:
```                
arr[3] = f(x);
```

The following, however, is invalid if x is a variable:

```
arr[x] = f(x);
```

As addition of `pointer`s is a library function rather than a language builtin. (It is not possible to generate the above code statically). This can be written as the following:

```
write(addPointer(arr, x), f(x));
```
Using addPointer() from the stdmath library.

Array initialisers are not supported:

```
var `int` numbers[5] = {1, 2, 3, 4, 5};
```
Because this list syntax is actually a special case of a string literal, so is of type `pointer` (consider that the table must be stored in ROM).

The following:

```
var `pointer` numbers = {1, 2, 3, 4, 5};
```

And

```
                    memcpy(numbers, {1, 2, 3, 4, 5}, 5);
```

Are both perfectly valid (using memcpy from the stdmem library).

<h3>
    <a name="_Toc376623631">Strings</a>
</h3>

Strings are constant arrays in ROM. When evaluated in expressions, they result in `pointer`s. Attempting to write to these `pointer`s is valid code, but will not result in any change (it’s called read only memory for a good reason).


These:

````
var `pointer` str = "Hello!";
````

And

```
var `pointer` str = {'H', 'e', 'l', 'l', 'o', 33, 0};
```

Are the same (ASCII, null-terminated).


String literals are immutable (as a consequence of the laws of physics as well as language semantics) – in order to perform mutating string operations you must first copy the string `int`o a memory buffer using strcpy() or similar.

```
var char buffer[64];
var `pointer` p = buffer;
p = strcpy(p, "I’m going to ");
p = strcpy(p, "concatenate some strings!");
printline(buffer);
```
<h3>
    <a name="_Toc376623632">Functions</a>
</h3>

Functions are defined with the following syntax:
```
function `int` f(int x)
{
    f = increment(shiftleft(x));
}
                
```
Note that “BASIC-style” returns are used (assign to function name). The return statement does exist, but it takes no arguments and simply exits the function.


The return type is optional –if omitted, the function returns `void`.


Arguments are passed by value, so the following:

```
function `int` sumto(n)
{
    sumto = 0;
    while (n)
    {
        sumto = add(sumto, n);
        n = decrement(n);
    }
}
```

Does not modify the original expression for n.

Recursion is not supported, as there is no machine stack – however, any recursive algorithm can be easily converted `int`o an iterative one through the use of a stack, and stacks are easy to implement with `pointer`s. Indirection to the rescue.

Memory allocated in one function is guaranteed to not be touched by another function (unless other functions do `pointer` arithmetic and don’t use bounds checking). This means all variables are the equivalent of static variables in C. See the following:

```
function `int` howManyTimesHaveIBeenCalled(int initialise)
{
    if (initialize)
        howManyTimesHaveIBeenCalled = 0;
    else
        howManyTimesHaveIBeenCalled = increment(HowManyTimesHaveIBeenCalled);
}
                
```
(This staticness includes the function variable). After having been called initially with an argument of true, the above function will return 1, then 2, then 3… etc.

<h3>
    <a name="_Toc376623633">Operators and Operator Precedence</a>
</h3>

There is no operator precedence. The only operators which are syntactically separate, as opposed to builtins using the function call syntax, are &amp;&amp; and || , which are really control flow operators; they operate based on short-circuit behaviour, as in C, Python, Lua and other languages:

- &amp;&amp;: evaluate the first argument. If it is true, evaluate the second argument and return it. If the first argument is false, return false
immediately.
- ||: evaluate the first argument. If it is true, return true immediately. If it is false, evaluate the second argument and return it.


These are useful if you want to make a decision based on a function of a `pointer`, but are not sure whether the `pointer` is valid(non-null):

```
if (pobject && some_function_of(pobject))
    do_something_to(pobject);
```

The function of the object will only be evaluated if the pointer is non-null.


These operators have equal precedence, and are left-associative. Precedence can be changed by the use of brackets to group expressions (see the logic operator test program).

<h3>
    <a name="_Toc376623634">Control Flow</a>
</h3>

Besides &amp;&amp; and ||, the available control flow keywords are `if`, `else`, `while`, `break`, `continue`, `return` and `goto`. These are almost entirely “borrowed” from C.


`if` and `else` work as expected. The `else` clause is optional. If the result of the `if` expression is non-zero, then the `if` branch is taken. If false, the `else` branch is taken, or the `if` branch is simply skipped if the `else` clause is omitted. The only quirk is that, if the expression results in a multi-byte value (i.e. a `pointer`), the decision is made based on the first (most significant) byte only. Use the `first()` and `second()` builtins if this is not the behaviour you’re after.

As a consequence of the fact that a block can be either a statement or multiple statements between braces, we can “chain up” ifs and elses to make an else if:

```
if (a)
    func_a();
else if (b)
    func_b();
else
    func_c();
```

There is in fact no else if keyword. If the above were written out fully, it would look like this:

```
if (a)
{
    func_a();
}
else
{
    if (b)
    {
        func_b();
    }
    else
    {
        func_c();
    }
}
```
`while` loops run the loop body for as long as the result of the expression is non-zero (with the same caveat as the if expression). If the expression is
zero upon the first evaluation, the loop body is not entered at all. The continue statement jumps back up to the test expression (e.g. when we want to move
on the the next item in a list) and the break statement exits the current loop.

The `return` statement exits a function (jumps to the epilogue) but, as noted in the function specification, it does not take an argument, as value returns are achieved by assigning to the function variable.

`goto` jumps to an address or label:

```
loop:
    x = increment(x);
    goto loop;
```

`goto`s are not very useful on their own, and in fact it is generally best to avoid them (see: Djikstra, Goto Considered Harmful). However, in conjunction with macros and the existing control flow operators, it is possible to implement entirely new control flow operators from within the language, as per TAGBODYs and GOs in Lisp.

Labels follow lexical scope, in the same way as variables.  

<h3>
    <a name="_Toc376623635">Macros</a>
</h3>
<h3>
    <a name="_Toc376623636">Builtin Functions</a>
</h3>

To make the language useful, a number of functions are built-in. These are usually sufficiently small to be generated in-place (and will be, for reasons of speed), and so do not use the regular calling convention.

<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td valign="top">
                
                    <strong>Name (Signature)</strong>
                
            </td>
            <td valign="top">
                
                    <strong>Description</strong>
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int and(int, `int`)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the bitwise and of two `int`egers. E.g. and(3, 5) = 1.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int andnot(int, `int`)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the bitwise and of the first `int`eger and the complement of the second. This takes only three machine instructions (the same as a
                    byte copy).
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int decrement(int)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the value of the argument – 1. Implemented as a table lookup.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int first(pointer)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the first (big endian =&gt; more significant) byte of a `pointer`.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>`void` nfc(ref, ref)</strong>
                
            </td>
            <td valign="top">
                
                    Compiles down to one single NFC machine instruction. This one is a special case – arguments are evaluated as follows:
                
                
                    · Functions are called and the return address ends up in the NFC instruction.
                
                
                    · Variables’ addresses end up in the NFC instruction.
                
                <ul>
                    <li>
                        Literal numbers are used directly as addresses.
                    </li>
                </ul>
                
                    In other words, everything is as standard unless you pass a number or constant in directly, e.g. nfc(debugout,debugin), in which case it
                    will use that constant as an address.
                
                
                    To perform NFCs with a value and not an address, use the val(int) operator.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>`void` nfc4(ref, ref, ref, ref)</strong>
                
            </td>
            <td valign="top">
                
                    Same as nfc(ref, ref) but arguments for all 4 fields are supplied, as opposed to the compiler stitching in the next instruction
                    automatically.
                
                
                    Used in combination with macros and labels, you can create your own control structures.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int not(int)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the bitwise complement of the argument, e.g. not(0) = 0xff.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int or(int, `int`)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the bitwise or of its arguments.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>pointer pair(int, `int`)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the `pointer` value that results from concatenating its two arguments. The first argument is the more significant, i.e. first in
                    memory (big endian).
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int read(pointer)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the result of reading from the passed `pointer` (dereferences the `pointer`).
                
                
                    If a literal number is passed in as the address this will compile down to <em>0</em> instructions. (This is still semantically significant,
                    e.g. compare the meaning of “x = debugin;” and “x = read(debugin);”: these are both single instructions but one assigns 0xc001 to x whilst
                    the other reads a value from the debug input port).
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int second(pointer)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the second (less significant) byte of a `pointer`.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int shiftleft(int)</strong>
                
            </td>
            <td valign="top">
                
                    Shifts its argument to the left by one bit, e.g. shiftleft(10) = 20. Implemented as a table lookup.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int shiftright(int)</strong>
                
            </td>
            <td valign="top">
                
                    Shifts its argument to the right by one bit, e.g. shiftright(10) = 5. Implemented as a table lookup.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>pointer val(int)</strong>
                
            </td>
            <td valign="top">
                
                    Takes an 8 bit integer as its input and returns an address that corresponds to that integer, e.g. “nfc(debugout, val(0xff));” will clear
                    the debug output port.
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>`void` write(pointer, `int`)</strong>
                
            </td>
            <td valign="top">
                
                    Writes a byte to a given location (lval `pointer` dereferencing).
                
            </td>
        </tr>
        <tr>
            <td valign="top">
                
                    <strong>int xor(int, `int`)</strong>
                
            </td>
            <td valign="top">
                
                    Returns the logical xor of the two arguments.
                
            </td>
        </tr>
    </tbody>
</table>

Other functions such as addition, copying chunks of memory, finding the length of strings etc. will not be built `int`o the compiler, but instead be part of the standard library. A complete listing of the standard library can be found in Appendix 2.

<h3>
    <a name="_Toc376623637">Headers and Libraries</a>
</h3>
