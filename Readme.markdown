Nor and Fork Conditionally
==========================
<img src="http://i.imgur.com/VmuX0j7.jpg">
*computer*
<img src="http://i.imgur.com/0p53ZI2.png">
*emulator*

Inspired by Jack Eisenman's DUO Compact CPU - ostracodfiles.com

This repo contains files relating to the One Instruction Set Computer, NFC. It contains:

- A javascript-based compiler for a low-level language, targeted at an NFC machine _(deprecated)_
- A C++ application for emulating such a machine
- A logisim file containing a possible implementation of an NFC machine
- Implementation notes for this physical machine
- A C++ compiler for a higher level language
- An Arduino serial-console-interface program for programming the computer

Thanks go to Jack for the excellent documentation on his site.

Instruction Set Description
---------------------------

As noted, the machine is a One Instruction Set Computer, or an OISC - it is only capable of executing one type of instruction. With a careful choice of instruction the machine is made universal; note that this does not make it practical!  
The chosen instruction is Nor and Fork Conditionally, or NFC - it loads two bytes from memory, NORs them together, stores the result in memory, and skips (forks) to one of two addresses depending on the result.  
The NOR operation is chosen as it is universal - with enough NORs, you can substitute for any other logical operation.  
As this is the only instruction, the instruction is implicit -- i.e., does not have to be specified to the processor -- and is made up entirely of 4 16 bit addresses: A, B, C, D. Broken down into steps:

1. `X = ~(A | B)		//NOR A and B`
2. `A = X                 	//Store in first location`
3. Fork conditionally
    - if result non-zero:
        - `Program Counter = C`
    - else
        - `Program Counter = D`

This single instruction cycle is made up of several microcycles - see the processor directory for further details.
Note that the instruction provides all of the necessary operations:

- Reading from memory:
   - Locations A and B are read in step 1
- Manipulating data:
    - NOR can be used to carry out any logic operation
- Writing to memory:
    - A is written to in step 2
- Branching
    - Conditional or non-conditional (if both skip addresses are the same)

It is also possible to read/write to runtime-specified locations (indirect addressing): although the addresses are given in absolute terms, self-modifying code can be written that will set the address fields of an instruction in RAM before executing it.

Machine Description
-------------------

The machine is implemented as an 8-bit machine, with a 16 bit address space. Addresses are big-endian. The address space is arranged as such:

- 0x0000 -> 0x7fff: ROM
	- 32KiB, immutable to machine.
- 0x8000 -> 0xbfff: RAM
	- 16KiB, writable by machine programs
	- may expand to 24KiB by changing address decode logic
- 0xc000 -> 0xffff: I/O
	- 0xc000: debug out
		- write-only, always reads as 0. Connected to 8 LEDs.
	- 0xc001: debug in
		- read only, connected to 8 DIP switches.
	- really a waste of addresses, might shrink this to make more room for RAM.

8 LEDs (memory mapped) provide output, and a bank of 8 DIP switches allows a 1-byte value to be input.
The architecture makes it easy to add more I/O - eventually there'll be a character LCD (which will need software to drive) and possibly a large flash device (~1MiB) for files and programs.
