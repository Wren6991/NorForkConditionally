///// This is a chronological document: new ideas are appended to the bottom as I make them up, so the top may not reflect the current design.


Steps:
1.  fetch bytes (we should fetch first byte last, so its location is still in the latch for writing
	-  A <- (PC & ~0x07 | 0x02)			// last byte first!
	-  Y <- (A)
	-  A <- (PC & ~0x07 | 0x00)
	-  X <- (A)
2.  NOR them, store in first location
	-  (A) <- [ALU]
3.  conditionally branch to 3rd or 4th address
	- A <- PC
	- ^PC <- (A & ~0b111 | 0b0Z0)
	- _PC <- (A & ~0b111 | 0b0Z1)

Note that the address register and PC must be inline-modified at read time; the suffix-select logic should be placed in line with the address latch and PC, upstream of the output buffer.
Note also that A is set to the *contents* of PC; this means set A to PC, load (A) into scratch registers, and load scratch registers into A.
The upper 2 bits of the address latch should go to the select input of a 2-4 demux; a general WE signal from microcode should be the demux's input. Microcode sends a write signal, and the demux routes the signal to the appropriate mapped device. The 11 (high mapping) output can serve as a highmem WE signal, feeding into decoding for IO devices.
As each address assignment is a 16 bit load, it must be done 8 bits at a time. I.e., 2 micro cycles.


Microcode:
Format:
ZAAAAA-> OoooWwww SsssNMmR (6 bit address -> 16 bit micro instruction)
Z: zero detect bit (address msb - microcode forking)
A: microcode address bits
O: general output enable bit
o: output enable select bits
W: general write enable bit
w: write enable select bits
S: suffix selector (0 = address suffix, 1 = microcode suffix)
s: suffix bits (least significant bits of address)
N: NOR gate buffer OE
M: mapped memory WE
m: mapped memory OE
R: reset microcode counter

WE/OE register mapping:
0 -> X
1 -> Y
2 -> HPC
3 -> LPC
4 -> HADDR
5 -> LADDR
6 -> scratch 1
7 -> scratch 0

/// if the suffix logic is only on the address latch:

Microcode listing:
ac00
bd00		; load high and low bytes of PC into A
0ea2		; write to scratch 1 from mapped memory, suffix 2 (a == 8 | 2)
0fb2		; write to scratch 0 from mapped memory, suffix 3: s1, s0 now contain the second operand address.
ec00
fd00		; copy scratch bytes into address register
0902		; read from mapped memory into Y operand
ac00
bd00		; copy PC into A
0e82		; copy memory to scratch, suffix 0
0f92		; copy memory to scratch, suffix 1
ec00
fd00		; copy scratch to A
0802		; read from mapped memory into X operand
000c		; copy NOR value back to memory
ac00
bd00		; copy PC into A
0ac2		; copy memory to ^PC, suffix 4 (or 6 if Z detect)
0bd2		; copy memory to _PC, suffix 5 (or 7 if Z detect)
0001		; reset counter						total length, 20 microcycles.




Possible modification:

Detach PC latches' outputs from the data bus, and put them on the address bus, upstream of suffix application:

                 |OE
+-------------+  v   |           |CONTROL     |
|     PC      |----->|           v            |
+-------------+      |     +------------+     |
                 |OE |---->|SUFFIX LOGIC|---->|
+-------------+  v   |     +------------+     |
|ADDRESS LATCH|----->|                        |
+-------------+      |                        |
    premodified address^               address bus^

We no longer need individual OEs for the PC latches, although we do need an OE signal for the address latches as they are no longer always attached to the bus.
As the PC is now directly routed to the address bus, we don't have to copy the PC values to the address latches before reading - this saves 6 microcycles per instruction!
Also note that this means we no longer need the scratch registers? (check on this!)
As the instruction locations we're reading from are transient values created by applying a suffix to the PC registers, we no longer need to store this location in the latches - we can generate it on the fly.
New execution order:
- A = (PC & ~0x07 | 0x02)		// 16 bit load: 2 ucycles
- Y = (A)				// 8 bit load: 1 ucycle
- A = (PC & ~0x07)			// 2 ucycles
- X = (A)				// 1 ucycle
- (A) = ~(X | Y)			// 1 ucycle
- A = (PC & ~0x07 | 0b1Z0)		// 2 ucycles     (note use of zero bit - this is a branch, but we can do this by feeding the 0 detect bit into the MSB of the microcode address.)
- PC = A				// 2 ucycles; 16 bit load, this will require more suffix logic.
Total: 11 cycles. Nearly twice as fast!
This means we need different mappings for OE and WE signals.
+----+--------+--------+------+
|Bits|   OE   |   WE   |nibble|
+----+--------+--------+------+
|0x00|        |   X    |  8   |
+----+--------+--------+------+
|0x01|        |   Y    |  9   |
+----+--------+--------+------+
|0x02|        |  ^PC   |  a   |
+----+--------+--------+------+
|0x03|        |  _PC   |  b   |
+----+--------+--------+------+
|0x04| ^ADDR  | ^ADDR  |  c   |
+----+--------+--------+------+
|0x05| _ADDR  | _ADDR  |  d   |
+----+--------+--------+------+
|0x06|   PC   |        |  e   |
+----+--------+--------+------+
|0x07|  ADDR  |        |  f   |
+----+--------+--------+------+

We could put the memory OE/WE signals in there as well, saving 2 bits of microcode space... however it also adds a 20ns delay to what is already the slowest part of the system, so keep it horizontal for now.
Revised microcode listing:
eca2    // write to HADDR from (PC), suffix = 0xa & 0x7 = 0x2
edb2    // write to LADDR
f902    // Y = (A)
ec82    // HADDR = (PC), suffix 0
ed92    // LADDR = (PC), suffix 1
f802    // X = (A)
f00c    // (A) = ~(X | Y)
ecc2    // or ece2 if Z;  HADDR = (PC), suffix 4/6
edd2    // or edf2 if Z;  LADDR = (PC), suffix 5/7
ca00    // HPC = HADDR
db00    // LPC = LADDR
0001    // reset
11 + reset = 12 microcycles total - originally 20, so 40% reduction in cycle time.

Further modification: transfer PC to ADDR through the _address_ bus, so we perform a 16 bit parallel load.
PC is only written to from the address latches anyway, so this won't affect its functionality.
Tie PC's inputs and outputs to the ADDR_DIRECT line, and have only a single WE signal for the entire 16 bit register - PC loads will then take 1 ucycle instead of 2.

0xe is now the write PC signal, 0xa and 0xb are unmapped.
This also means we no longer need individual OE signals for HADDR and LADDR... it's effectively a write only register now, which we can transfer to PC if we want to.

So this would make the following listing:

eca2
edb2
f902
ec82
ed92
f802
f00c
ecc2	; ce if Z
edd2	; df if Z
fe00
0001

Note that only e and f are given for first nibble (besides 0): this is because we are only controlling output from the PC and ADDR, so it would make more sense to use 2 horizontal bits than 4 encoded ones!
Same goes for write bits: c, d, 8, 9, e for ^ADDR, _ADDR, X, Y and PC. Better to use 5 horizontal bits (at the cost of an extra bit), and take a 12ns decoding delay out of the critical timing path of the CPU.
So we go from 8 bits to 7, take 2 chips out, and gain 1 chip delay of cycle time.

List of microcode signals:
wX, wY, w^AD, w_AD, wPC, wMEM, oAD, oPC, oMEM, oALU, sSUF, SUF2, SUF1, SUF0, mRST

15 bits, split between 2 8bit wide PROMs.
Ideally:
- write and output signals on different PROMs so we can stagger them
- reset signal on the PROM that gets triggered first, so we can skip back straight away. (If necessary hardcode the reset signal from the counter and get rid of the soft reset signal)
- the suffix must be applied no later than wMEM going high - else we will not be writing to the correct location!
    - take a look at the timing diagrams to see how much headroom we have
    - if necessary, buffer the 4 OE signals individually rather than at a PROM level, seeing as the suffix and OE bits are probably going to end up on the same PROM.
    - actually this bit isn't that important because we only modify the suffix while we're reading from memory. (Still might be worth bringing the address latch straight past the encoder? eh.)

Possible bit organisation:
+-----+-----------------+
| Bit | PROM 1 | PROM 0 |
+-----+--------+--------+
|  7  |  oAD   |   --   |
|  6  |  oPC   |  wPC   |
|  5  |  oMEM  |  wMEM  |
|  4  |  oALU  |  w^AD  |
|  3  |  SUFs  |  w_AD  |
|  2  |  SUF2  |  wX    |
|  1  |  SUF1  |  wY    |
|  0  |  SUF0  |  RST   |
+-----+--------+--------+
                                   __  
Output bits need inverting anyway (OE, not just OE), so they'll be individually buffered as it is. No need to trigger the PROMs separately - timing allowing.

New listing (binary): oooossss _wwwwwwr

0000: 0110 1010 0001 0000		; HADDR <- (PC + 2)
0001: 0110 1011 0000 1000		; LADDR <- (PC + 3)
0010: 1010 0000 0000 0010		; Y <- (ADDR)
0011: 0110 1000 0001 0000		; HADDR <- (PC + 0)
0100: 0110 1001 0000 1000		; LADDR <- (PC + 1)
0101: 1010 0000 0000 0100		; X <- (ADDR)
0110: 1001 0000 0010 0000		; (ADDR) <- ALU
0111: 0110 11Z0 0001 0000		; HADDR <- (PC + 4 + 2*Z)
1000: 0110 11Z1 0000 1000 		; LADDR <- (PC + 5 + 2*Z)
1001: 1000 0000 0100 0000		; PC <- ADDR
1010: 0000 0000 0000 0001		; reset

Same listing in hex:
6a10, 6b08, a002, 6810, 6908, a004, 9020, 6c10, 6d08, 8040, 0001
Or, if Z:             ''                  6e10, 6f10,     ''
