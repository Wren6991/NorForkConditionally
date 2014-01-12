<h2>
    <a name="_Toc376623627">Language Specification</a>
</h2>
*Disclaimer: This is ripped straight from my A level writeup (a Word document), so some of the formatting's probably messed up and the appendices don't exist. I'll sort this out!*
<p>
    The language is called Spoon. (As the successor to Fork, this was the only logical choice, besides “Knife”. Knife would be banned in schools anyway, unless
    I rounded the ends and made it out of plastic.)
</p>
<p>
    The syntax of the language is defined by the following BNF/regular expressions:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    &lt;program&gt; ::= {&lt;constdef&gt;|&lt;funcdef&gt;|&lt;macrodef&gt;|&lt;vardecl&gt;}
                </p>
                <p>
                    &lt;constdef&gt; ::= `const` &lt;type&gt; &lt;name&gt; `=` &lt;expression&gt; `;`
                </p>
                <p>
                    &lt;funcdef&gt; ::= `function` [&lt;type&gt;] &lt;name&gt; `(` &lt;type&gt; &lt;name&gt; {`,` &lt;type&gt; &lt;name&gt;} `)` &lt;block&gt;
                </p>
                <p>
                    &lt;type&gt; ::= `int`|`pointer`|`void`
                </p>
                <p>
                    &lt;macrodef&gt; ::= `macro` &lt;name&gt; `(` &lt;name&gt; {`,` &lt;name&gt;} `)` &lt;block&gt;
                </p>
                <p>
                    &lt;block&gt; ::= &lt;statement&gt;
                </p>
                <p>
                    | `{` {&lt;vardecl&gt;} {&lt;statement&gt;} `}`
                </p>
                <p>
                    &lt;vardecl&gt; ::= &lt;type&gt; &lt;varname&gt; {`,` &lt;varname&gt;} `;`
                </p>
                <p>
                    &lt;varname&gt; ::= &lt;name&gt; [`[` &lt;number&gt; `]`] [`=` &lt;expression&gt;]
                </p>
                <p>
                    &lt;statement&gt; ::= &lt;name&gt; `(` &lt;expression&gt; {`,` &lt;expression&gt;} `)` `;`
                </p>
                <p>
                    | &lt;name&gt; `=` &lt;expression&gt; `;`
                </p>
                <p>
                    | `if` `(` &lt;expression&gt; `)` &lt;block&gt; [`else` &lt;block&gt;]
                </p>
                <p>
                    | `while` `(` &lt;expression&gt; `)` &lt;block&gt;
                </p>
                <p>
                    | &lt;name&gt; `:`
                </p>
                <p>
                    | `goto` &lt;name&gt; `;`
                </p>
                <p>
                    | (`break` | `continue` | `return`) `;`
                </p>
                <p>
                    &lt;expression&gt; ::= &lt;value&gt; [(`&amp;&amp;` | `||`) &lt;expression&gt;]
                </p>
                <p>
                    &lt;value&gt; ::= `!`&lt;value&gt;
                </p>
                <p>
                    | `(`&lt;expression&gt;`)`
                </p>
                <p>
                    | &lt;name&gt;
                </p>
                <p>
                    | &lt;name&gt; `(`&lt;expression&gt; {`,` &lt;expression&gt;}`)`
                </p>
                <p>
                    | &lt;name&gt; `[`&lt;number&gt;`]`
                </p>
                <p>
                    | &lt;string&gt;
                </p>
                <p>
                    | `{`&lt;number&gt; {`,` &lt;number&gt;} `}`
                </p>
                <p>
                    | &lt;number&gt;
                </p>
                <p>
                    &lt;name&gt; ::= [a-zA-Z_][a-zA-Z0-9_]*
                </p>
                <p>
                    &lt;number&gt; ::= (0x[0-9a-fA-F]+)|([0-9]+)
                </p>
                <p>
                    | `'`&lt;char&gt;`'`
                </p>
                <p>
                    &lt;string&gt; ::= \"[^\"]*\"
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Where non-terminals are in &lt;angle-brackets&gt; , terminals are in `backquotes`, [x] represents optional x and {x} represents optional multiple x (a
    Kleene closure).
</p>
<p>
    Identifiers and operators are all case sensitive.
</p>
<p>
    A program is a set of declarations: of functions, of macros, of variables and of constants. The only stipulation is that there must exist a function called
    main, which the linker will use as the program entry point. main takes no arguments and returns void.
</p>
<h3>
    <a name="_Toc376623628">Types</a>
</h3>
<p>
    There are 3 main types: void, int and pointer. You can also use the name char instead of int, or int16 instead of pointer, but they are logically and
    internally the same.
</p>
<p>
    void is a pseudo-type, and represents the result of a function that does not return anything. You can define a variable of type void, but trying to assign
    anything but the result of a void function to it will generate a type error. (Why you would want to do this is beyond me, but it is logically consistent).
    It has a size of 0 bytes.
</p>
<p>
    int has a size of one byte (8 bits). It is the basic unit of arithmetic, and can also represent a single ASCII character. The nickname char is a completely
    equivalent type, but is a little more natural when writing text processing code (a la C).
</p>
<p>
    pointer is 2 bytes long, or 16 bits. It refers to a single 8-bit word of machine memory, or can be used to represent a large integer, such as the size of a
    file in bytes (hence the pseudonym int16).
</p>
<p>
    The only compound type is the array. If you need to represent a complex data structure (like C’s struct), use an array.
</p>
<h3>
    <a name="_Toc376623629">Variable Declarations and Scope</a>
</h3>
<p>
    Variable declarations can take place anywhere inside a block, but scoping is at a block-level – this has the effect that all variable declarations are
    processed upon entering a block.
</p>
<p>
    Multiple variables of the same type can be declared at once, and each variable can be initialised with any valid expression:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var int x = 5, y = f(x);
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    The initialisations are guaranteed to be performed left-to-right, so f will receive an argument of 5 in the above example.
</p>
<p>
    While the declarations are processed as though they were at the top of a block, the initializations are compiled in the position they are written, so there
    is no loss of sequencing.
</p>
<p>
    Variables are lexically scoped, as per Algol, C, Haskell and Common Lisp. For example:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    function main()
                </p>
                <p>
                    {
                </p>
                <p>
                    var int a, b, c;
                </p>
                <p>
                    a = 5;
                </p>
                <p>
                    if (true)
                </p>
                <p>
                    {
                </p>
                <p>
                    var int a;
                </p>
                <p>
                    a = increment(a);
                </p>
                <p>
                    }
                </p>
                <p>
                    output(a, debugout);
                </p>
                <p>
                    }
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Will result in an output of 5, because the new a defined inside the if block “shadows” the old one, so the original a is unchanged.
</p>
<h3>
    <a name="_Toc376623630">Arrays</a>
</h3>
<p>
    The language allows you to define arrays:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var int arr[5];
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    An area of size n * sizeof(type) bytes is allocated by the linker and guaranteed not to be touched by other variables for the duration of the current
    block, e.g. an array of 5 pointers would be 10 bytes. You can assign into arrays if you have a constant offset:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    arr[3] = f(x);
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    The following, however, is invalid if x is a variable:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    arr[x] = f(x);
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    As addition of pointers is a library function rather than a language builtin. (It is not possible to generate the above code statically). This can be
    written as the following:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    write(addPointer(arr, x), f(x));
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Using addPointer() from the stdmath library.
</p>
<p>
    Array initialisers are not supported:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var int numbers[5] = {1, 2, 3, 4, 5};
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Because this list syntax is actually a special case of a string literal, so is of type pointer (consider that the table must be stored in ROM).
</p>
<p>
    The following:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var pointer numbers = {1, 2, 3, 4, 5};
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    And
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var int numbers[5];
                </p>
                <p>
                    memcpy(numbers, {1, 2, 3, 4, 5}, 5);
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Are both perfectly valid (using memcpy from the stdmem library).
</p>
<h3>
    <a name="_Toc376623631">Strings</a>
</h3>
<p>
    Strings are constant arrays in ROM. When evaluated in expressions, they result in pointers. Attempting to write to these pointers is valid code, but will
    not result in any change (it’s called read only memory for a good reason).
</p>
<p>
    These:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var pointer str = "Hello!";
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    And
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var pointer str = {'H', 'e', 'l', 'l', 'o', 33, 0};
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Are the same (ASCII, null-terminated).
</p>
<p>
    String literals are immutable (as a consequence of the laws of physics as well as language semantics) – in order to perform mutating string operations you
    must first copy the string into a memory buffer using strcpy() or similar.
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    var char buffer[64];
                </p>
                <p>
                    var pointer p = buffer;
                </p>
                <p>
                    p = strcpy(p, "I’m going to ");
                </p>
                <p>
                    p = strcpy(p, "concatenate some strings!");
                </p>
                <p>
                    printline(buffer);
                </p>
            </td>
        </tr>
    </tbody>
</table>
<h3>
    <a name="_Toc376623632">Functions</a>
</h3>
<p>
    Functions are defined with the following syntax:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    function int f(int x)
                </p>
                <p>
                    {
                </p>
                <p>
                    f = increment(shiftleft(x));
                </p>
                <p>
                    }
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Note that “BASIC-style” returns are used (assign to function name). The return statement does exist, but it takes no arguments and simply exits the
    function.
</p>
<p>
    The return type is optional –if omitted, the function returns void.
</p>
<p>
    Arguments are passed by value, so the following:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    function int sumto(n)
                </p>
                <p>
                    {
                </p>
                <p>
                    sumto = 0;
                </p>
                <p>
                    while (n)
                </p>
                <p>
                    {
                </p>
                <p>
                    sumto = add(sumto, n);
                </p>
                <p>
                    n = decrement(n);
                </p>
                <p>
                    }
                </p>
                <p>
                    }
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Does not modify the original expression for n.
</p>
<p>
    Recursion is not supported, as there is no machine stack – however, any recursive algorithm can be easily converted into an iterative one through the use
    of a stack, and stacks are easy to implement with pointers. Indirection to the rescue.
</p>
<p>
    Memory allocated in one function is guaranteed to not be touched by another function (unless other functions do pointer arithmetic and don’t use bounds
    checking). This means all variables are the equivalent of static variables in C. See the following:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    function int howManyTimesHaveIBeenCalled(int initialise)
                </p>
                <p>
                    {
                </p>
                <p>
                    if (initialize)
                </p>
                <p>
                    howManyTimesHaveIBeenCalled = 0;
                </p>
                <p>
                    else
                </p>
                <p>
                    howManyTimesHaveIBeenCalled = increment(HowManyTimesHaveIBeenCalled);
                </p>
                <p>
                    }
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    (This staticness includes the function variable). After having been called initially with an argument of true, the above function will return 1, then 2,
    then 3… etc.
</p>
<h3>
    <a name="_Toc376623633">Operators and Operator Precedence</a>
</h3>
<p>
    There is no operator precedence. The only operators which are syntactically separate, as opposed to builtins using the function call syntax, are &amp;&amp;
    and || , which are really control flow operators; they operate based on short-circuit behaviour, as in C, Python, Lua and other languages:
</p>
<p>
    · &amp;&amp;: evaluate the first argument. If it is true, evaluate the second argument and return it. If the first argument is false, return false
    immediately.
</p>
<p>
    · ||: evaluate the first argument. If it is true, return true immediately. If it is false, evaluate the second argument and return it.
</p>
<p>
    These are useful if you want to make a decision based on a function of a pointer, but are not sure whether the pointer is valid(non-null):
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    if (pobject &amp;&amp; some_function_of(pobject))
                </p>
                <p>
                    do_something_to(pobject);
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    The function of the object will only be evaluated if the pointer is non-null.
</p>
<p>
    These operators have equal precedence, and are left-associative. Precedence can be changed by the use of brackets to group expressions (see the logic
    operator test program).
</p>
<h3>
    <a name="_Toc376623634">Control Flow</a>
</h3>
<p>
    Besides &amp;&amp; and ||, the available control flow keywords are if, else, while, break, continue, return and goto. These are almost entirely “borrowed”
    from C.
</p>
<p>
    if and else work as expected. The else clause is optional. If the result of the if expression is non-zero, then the if branch is taken. If false, the else
    branch is taken, or the if branch is simply skipped if the else clause is omitted. The only quirk is that, if the expression results in a multi-byte value
    (i.e. a pointer), the decision is made based on the first (most significant) byte only. Use the first() and second() builtins if this is not the behaviour
    you’re after.
</p>
<p>
    As a consequence of the fact that a block can be either a statement or multiple statements between braces, we can “chain up” ifs and elses to make an else
    if:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    if (a)
                </p>
                <p>
                    func_a();
                </p>
                <p>
                    else if (b)
                </p>
                <p>
                    func_b();
                </p>
                <p>
                    else
                </p>
                <p>
                    func_c();
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    There is in fact no else if keyword. If the above were written out fully, it would look like this:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    if (a)
                </p>
                <p>
                    {
                </p>
                <p>
                    func_a();
                </p>
                <p>
                    }
                </p>
                <p>
                    else
                </p>
                <p>
                    {
                </p>
                <p>
                    if (b)
                </p>
                <p>
                    {
                </p>
                <p>
                    func_b();
                </p>
                <p>
                    }
                </p>
                <p>
                    else
                </p>
                <p>
                    {
                </p>
                <p>
                    func_c();
                </p>
                <p>
                    }
                </p>
                <p>
                    }
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    while loops run the loop body for as long as the result of the expression is non-zero (with the same caveat as the if expression). If the expression is
    zero upon the first evaluation, the loop body is not entered at all. The continue statement jumps back up to the test expression (e.g. when we want to move
    on the the next item in a list) and the break statement exits the current loop.
</p>
<p>
    The return statement exits a function (jumps to the epilogue) but, as noted in the function specification, it does not take an argument, as value returns
    are achieved by assigning to the function variable.
</p>
<p>
    goto jumps to an address or label:
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="616" valign="top">
                <p>
                    loop:
                </p>
                <p>
                    x = increment(x);
                </p>
                <p>
                    goto loop;
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    gotos are not very useful on their own, and in fact it is generally best to avoid them (see: Djikstra, Goto Considered Harmful). However, in conjunction
    with macros and the existing control flow operators, it is possible to implement entirely new control flow operators from within the language, as per
    TAGBODYs and GOs in Lisp.
</p>
<p>
    Labels follow lexical scope, in the same way as variables.
</p>
<h3>
    <a name="_Toc376623635">Macros</a>
</h3>
<h3>
    <a name="_Toc376623636">Builtin Functions</a>
</h3>
<p>
    To make the language useful, a number of functions are built-in. These are usually sufficiently small to be generated in-place (and will be, for reasons of
    speed), and so do not use the regular calling convention.
</p>
<table border="1" cellspacing="0" cellpadding="0">
    <tbody>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>Name (Signature)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    <strong>Description</strong>
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int and(int, int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the bitwise and of two integers. E.g. and(3, 5) = 1.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int andnot(int, int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the bitwise and of the first integer and the complement of the second. This takes only three machine instructions (the same as a
                    byte copy).
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int decrement(int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the value of the argument – 1. Implemented as a table lookup.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int first(pointer)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the first (big endian =&gt; more significant) byte of a pointer.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>void nfc(ref, ref)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Compiles down to one single NFC machine instruction. This one is a special case – arguments are evaluated as follows:
                </p>
                <p>
                    · Functions are called and the return address ends up in the NFC instruction.
                </p>
                <p>
                    · Variables’ addresses end up in the NFC instruction.
                </p>
                <ul>
                    <li>
                        Literal numbers are used directly as addresses.
                    </li>
                </ul>
                <p>
                    In other words, everything is as standard unless you pass a number or constant in directly, e.g. nfc(debugout,debugin), in which case it
                    will use that constant as an address.
                </p>
                <p>
                    To perform NFCs with a value and not an address, use the val(int) operator.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>void nfc4(ref, ref, ref, ref)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Same as nfc(ref, ref) but arguments for all 4 fields are supplied, as opposed to the compiler stitching in the next instruction
                    automatically.
                </p>
                <p>
                    Used in combination with macros and labels, you can create your own control structures.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int not(int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the bitwise complement of the argument, e.g. not(0) = 0xff.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int or(int, int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the bitwise or of its arguments.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>pointer pair(int, int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the pointer value that results from concatenating its two arguments. The first argument is the more significant, i.e. first in
                    memory (big endian).
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int read(pointer)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the result of reading from the passed pointer (dereferences the pointer).
                </p>
                <p>
                    If a literal number is passed in as the address this will compile down to <em>0</em> instructions. (This is still semantically significant,
                    e.g. compare the meaning of “x = debugin;” and “x = read(debugin);”: these are both single instructions but one assigns 0xc001 to x whilst
                    the other reads a value from the debug input port).
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int second(pointer)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the second (less significant) byte of a pointer.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int shiftleft(int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Shifts its argument to the left by one bit, e.g. shiftleft(10) = 20. Implemented as a table lookup.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int shiftright(int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Shifts its argument to the right by one bit, e.g. shiftright(10) = 5. Implemented as a table lookup.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>pointer val(int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Takes an 8 bit integer as its input and returns an address that corresponds to that integer, e.g. “nfc(debugout, val(0xff));” will clear
                    the debug output port.
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>void write(pointer, int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Writes a byte to a given location (lval pointer dereferencing).
                </p>
            </td>
        </tr>
        <tr>
            <td width="206" valign="top">
                <p>
                    <strong>int xor(int, int)</strong>
                </p>
            </td>
            <td width="410" valign="top">
                <p>
                    Returns the logical xor of the two arguments.
                </p>
            </td>
        </tr>
    </tbody>
</table>
<p>
    Other functions such as addition, copying chunks of memory, finding the length of strings etc. will not be built into the compiler, but instead be part of
    the standard library. A complete listing of the standard library can be found in Appendix 2.
</p>
<h3>
    <a name="_Toc376623637">Headers and Libraries</a>
</h3>
