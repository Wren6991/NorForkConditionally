Benchmarking
============

Performance Benchmark:
---------------

```
const pointer debugout = 0xc000;
const pointer debugin = 0xc001;
const int true = 0xff;
const int false = 0x00;

function int decrement(int x);
function int read(pointer p);

function sleep(int time)
{
    while (time)
        time = decrement(time);
}

function main()
{
    while (true)
    {
        var int period;
        period = read(debugin);
        sleep(period);
        nfc(debugout, val(0xff));
        sleep(period);
        nfc(debugout, val(0x00));
    }
}
```

Size Benchmark:
---------------

```
const pointer debugout = 0xc000;
const pointer debugin = 0xc001;
const int true = 0xff;
const int false = 0x00;

function int read(pointer p);
function int increment(int x);
function int decrement(int x);
function int shiftleft(int x);
function int shiftright(int x);

macro output(src, dest)
{
    var int temp;
    nfc(temp, val(0xff));
    nfc(temp, src);
    nfc(dest, temp);
}

function int andnot(int a, int b)
{
    nfc(andnot, val(0xff));
    nfc(andnot, a);
    nfc(andnot, b);
}

function int and(int a, int b)
{
    nfc(and, val(0xff));
    nfc(and, a);
    nfc(b, b);
    nfc(and, b);
}

function int or(int a, int b)
{
    var int temp;
    nfc(or, val(0xff));
    nfc(or, a);
    nfc(or, or);
    nfc(or, b);
    nfc(or, or);
}

function int xor(int a, int b)
{
    var int temp1, temp2;
    nfc(temp1, val(0xff));
    nfc(temp1, a);
    nfc(temp1, b);
    nfc(temp2, val(0xff));
    nfc(temp2, b);
    nfc(temp2, a);
    nfc(xor, val(0xff));
    nfc(xor, temp1);
    nfc(xor, xor);
    nfc(xor, temp2);
    nfc(xor, xor);
}

function int equal(int a, int b)
{
    if (xor(a, b))
        equal = 0;
    else
        equal = 1;
}

function main()
{
    while (true)
    {
        var int x, op;
        x = read(debugin);
        op = and(x, 0xc0);
        x = and(x, 0x3f);
        var int temp;
        nfc(temp, val(0xff));
        if (equal(op, 0x00))
        {
            nfc(temp, increment(x));
        }
        else if (equal(op, 0x40))
        {
            nfc(temp, decrement(x));
        }
        else if (equal(op, 0x80))
        {
            nfc(temp, shiftleft(x));
        }
        else
        {
            nfc(temp, shiftright(x));
        }
        nfc(debugout, temp);
    }
}
```

Before optimization
-------------------

Frequency when debugin == 0:

- 4.167 kHz

Frequency when debugin == ff:

- 23.21 Hz

Size benchmark:

- 1984 bytes

Read becomes copy when address constant, 0-byte branchalways instructions:
--------------------------------------------------------------------------

Frequency when debugin == 0:

- 5.556 kHz

Frequency when debugin == ff:

- 25.68 Hz

Size benchmark:

- 1776 bytes

Fibonacci
---------

Read in 0xa from debugin, calculate the correct value of 0x37 and output to debugout.

Test code:

```
#include "stddefs"

function int add(int a, int b)
{
    var int carry, temp;
    add = xor(a, b);
    carry = shiftleft(and(a, b));
    while (carry)
    {
        temp = add;
        add = xor(add, carry);
        carry = shiftleft(and(temp, carry));
    }
}

function main()
{
    while (true)
    {
        var int a = 0, b = 1, c, count;
        count = read(debugin);
        while (count)
        {
            c = add(a, b);
            a = b;
            b = c;
            count = decrement(count);
        }
        output(a, debugout);
    }
}
```

- Initial timing:
	- 1.65 ms.
- Initial size:
	- 0x5f8

Added the "preferred target" thing.

- New timing:
	- 1.41 ms
- New size:
	0x530
	

Screen test
-----------

Version of code:
(https://github.com/Wren6991/NorForkConditionally/blob/b30ba520a1d9c13c1fbdd4a703a8ee707e8a7a9f/spoon/samples/screen.spn)screen.spn

- Before optimization:
	- size: 0x27b8
- Preferred location information:
	- size: 0x24f0
- Preferred location information for multi-byte constant temps (e.g. string addresses)
	- size: 0x23a0

