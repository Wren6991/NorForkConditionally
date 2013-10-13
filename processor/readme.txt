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
s
Same listing in hex:
6a10, 6b08, a002, 6810, 6908, a004, 9020, 6c10, 6d08, 8040, 0001
Or, if Z:             ''                  6e10, 6f10,     ''

Breadboard layout:
------------------

 Control   Memory  PC/ADDR    ALU      I/O    Storage
+--------+--------+--------+--------+--------+--------+
|Counter |WE dmux |ADDRmux |   X    |  SIG   |  OUT   |
|        |        |        |        |        |        |
|   u0   |OE dmux |  LPC   |   Y    |dipswtch|   IN   |
|        |        |        |        |        |        |
|   u1   |        |  HPC   | NOR0-3 | DBGIN  | FLASH  |
|        |        |        |        |        |        |
|Inverter|  ROM   | LADDR  | NOR4-7 | DBGOUT |   LO   |
|        |        |        |        |        |        |
|Osciltr.|  RAM   | HADDR  |ALUBUFF |   OE   |  MID   |
|        |        |        |        |        |        |
|        |        |        | NORall |   WE   |        |
+--------+--------+--------+--------+--------+--------+


Chip    | part no.
--------+-------------
Counter |74HC4520E
u0      |AT28HC291
u1	|   ''    
Inverter|74HC04
Osciltr.|   ''
WE dmux |74HC138
OE dmux |   ''
ROM	|AT28C256-15PU
RAM     |AS6C1008
ADDRmux |74HCT257
LPC     |74HC373
HPC     |   ''
LADDR   |   ''
HADDR   |   ''
X       |   ''
Y       |   ''
NOR0-3  |CD4001BE
NOR4-7  |   ''
ALUBUFF |74HC244
NORall  |MC14078B
SIG     |74HC373
DBGIN   |   ''
DBGOUT  |   ''
OE      |74HC138
WE      |74HC238
OUT     |74HC373
IN      |   ''
FLASH   |A29040B-70F
LO      |74HC373
MID     |   ''

Control unit
------------

The control unit consists of:

- A pair of four-bit counters
    - One is used for the microcode address, the other for frequency division
- A pair of 2 kilobyte EEPROMs for microcode storage
    - Format and contents were discussed earlier
- A pair of 74HC04 inverters
    - One is used for inversion/phase adjustment of signals
    - The other runs and buffers the 8MHz crystal oscillator (separate chip to avoid coupling into control signals)

The microcode ROMs assert their contents on the control bus while CLK is high; the microcode address counter increments when #CLK goes high.
Using the rising edge of the inverted signal instead of just the falling edge of the signal gives a few nanoseconds' hold-off between the ROMs' output buffers being turned off and the address changing, to avoid spurious signals.
During the "tock" phase of the clock, the control bus is not driven, and is pulled to ground by pulldown resistors. WEs have lower value resistors (~5k vs 100k) so they go low more quickly than OEs, helping data holdoff for register/memory writes.
Microcode address signals uA1 and uA3 (values 2, 8) are connected to a diode-resistor and gate:
    ^
    |  10k resistor
   [r]
rst-|-|>|- uA1    (  -|>|-  is a schottky diode  )
    |-|>|- uA2

Which is then connected to the counter's reset pin. The address changes while the clock is low, meaning the counter is reset during the quiescent "tock" phase and doesn't affect the assertion of the signals.
The only feedback from the rest of the computer to the control unit is the Z_DETECT line from the ALU - this feeds into the top bit of the microcode address, causing two different sequences of operations to be performed based on the result of the last ALU operation.

The second half of the counter chip (two four-bit counters on a chip) divides the 8MHz clock down to 4->2->1->0.5 MHz, which can be selected with a short jumper wire, although the system is now (fingers crossed) stable at 8MHz provided a reasonable power supply.

Memory board
------------

Not much to see here!
The ROM and RAM chip are connected to the data bus and the address bus - 15 bits of the address bus go to the ROM (32KiB) and 14 bits to the RAM (16KiB).
ROM, RAM and IO devices are memory mapped.
The top 2 bits of the address bus go to demultiplexers, which are activated by the general OE/WE signals, and select devices with a resolution of 16KiB.
The ROM is not writable (at least not by software!) so its #WE pin is held high by a 100k pullup. WE for segments 2 and 3 (8000->ffff) are routed to RAM and the IO board, respectively.
ROM spans two segments, so another schottky diode-resistor AND gate is used as a negative logic OR gate to smush the two #OE signals together. The diodes give a nice sharp falling edge, and the 10k pullup gives a slightly less sharp (few nanoseconds) rising edge.
The top segment signals (c000->ffff) goes off to the IO board, which is described later.

PC/ADDR board
-------------

This board contains the program counter, which is a single 16-bit register containing the address of the current instruction, and the address latch, which has 2 8-bit inputs and one 16-bit output, and addresses a byte to be read from memory.
The address latches load from the data bus, and output to something I call the immediate address bus (I need a better name!). This bus is internal to the PC/ADDR board.
The program counter loads from and outputs to the immediate address bus - it is loaded in a single 16-bit transfer from the address latch.
There is only one other chip on this board, and this chip accounts for the need for the immediate address bus: a multiplexer, called the suffix selector, selects the source of the least significant 3 bits of the outside address bus.
Why is this needed? When reading/writing to the location of the address latch, the suffix selector receives select signal 0: the immediate address provides the last 3 bits. In other words, the address of the RAM/ROM location, IO device or whatever is the same as the program counter / address latch. It's a non-operation.
When the select signal is 1, the _microcode_ provides the last 3 bits of the address. This is important, because the instructions are 8 (2 ** 3) instructions long.

                        |OE
       +-------------+  v   |           |CONTROL     |
       |     PC      |<---->|           v            |
       +-------------+      |     +------------+     |
                        |OE |---->|SUFFIX LOGIC|---->|
       +-------------+  v   |     +------------+     |
data-->|ADDRESS LATCH|----->|                        |
       +-------------+      |                        |
         premodified address^             address bus^

As a result, the microcode can select which of the 8 bytes that form the instruction it would like to address, relative to the current PC value.
The more signicant 13 bits are connected directly to the address bus.
The address bus doesn't have any pulldowns, as it's driven every clock cycle.

ALU board
---------

The ALU is very simple - it provides only one operation, bitwise NOR. The other chips store operands, interface with the data bus, and inform the control unit as to whether the last result was a zero.

- X, Y: these are the 8-bit operand registers. They are loaded from the data bus, and output directly to the NOR gates
- NOR0-3, NOR4-7: NOR gates that implement the ALU's only function. Their outputs go to the data buffer and the zero-detect logic
- ALUBUFF: an 8-bit buffer that asserts the result of the last operation on the data bus
- NORall: an 8-input NOR gate; when all its inputs are 0, its result is a one. Otherwise, it's a zero. Feeds back to the control unit

The control unit controls the WE and #OE signals directly (i.e. horizontal microcode).
The speed of the NOR gates is actually not very important, as the operands are present by the end of the preceding cycle to the result being used, and the NOR operation is completely combinational.

|  +---+              +---------------+
|->| X |   +-----\    |               v
|  +---+-->\      \   |  +-------+ +------+
|           > NOR  )o-+->|ALUBUFF| |NORall|
|  +---+-->/      /      +-------+ +------+
|->| Y |   +-----/           |         |
|  +---+                     v         +-> Z_DETECT 
+--------------------------------+
  data bus                       |

IO board
--------

Again, not much to see!
The board mostly consists of memory-mapped latches which are written to and read from by software.
The memory mapping is performed by a pair of 1->8 demultiplexers.
The #OE dmux is a 74137, the inverting type, while the WE dmux is a 74237, the non-inverting type. Previously I borrowed part of a nearby 7404 to invert the signal, but now that there is more than one output I thought I may as well buy the correct chip and take that link out of the timing chain (I never expected it to work at all with that inverter there).
It's of vital importance that the output latches read as all 0s, else the write will be corrupted; as they do not drive the data bus low when they are read (their outputs are pointed at the outside world) it wouldn't be unthinkable for spurious data to be left on the data bus at higher clock speeds, which would completely mess up the write operation.
It's for this reason that the data bus is pulled down rather aggressively, with 4k7 resistors.

The terms input and output are used relative to the computer, i.e. out is databus->peripheral and in is peripheral->databus:

- DBGOUT - output, address c000
    - Connected to 8 LEDs, and bits 7, 6, 3, 2, 1, 0 are connected to the LCD.
- DBGIN - input, address c001
    - Connected to an 8 bit dip switch. 100k resistors are more than sufficient for the pulldown, as human input will never exceed a few Hz.
- SIG - output, address c002
    - Ought really to be on the flash board but there wasn't quite space. There used to be a piezo transducer in this position, I need to find a smaller one to squeeze in.
    - Bits 2->0 form the most significant 3 bits of the flash address (512KiB so 19 bits)
    - Bit 7 is flash WE
    - Bit 6 is #OE for the flash chip; it is also inverted by an NPN transistor to give #OE for the flash input register (0 = flash->out, 1 = in->flash.) They're complementary to avoid contention. More of that later!

Flash board
-----------

This board contains a half-megabyte A29040B-70F (soon to be SST39SF040-70A, for 4k sectors instead of 64k) flash chip, and the input and output registers to control it.

- MID - output, address c003
	- bits 15-8 of the flash address
- LO - output, address c004
	- bits 7-0 of the flash address
- IN - output, address c005
	- Yes I know what I said about data direction, in this case "in" is relative to the flash chip.
	- Asserts its value on the flash IO pins when bit 6 of the SIG register is *high*. (#OE <= not SIG(6))
 - OUT - input, address c006
 	- Asserts flash IO pins onto data bus when read.
	- Its LE pin is tied high so it's really a buffer rather than a register, but it was convenient to use a 74373 and keep the same pinout.

The flash chip's #OE is tied to bit 6 of the SIG register - this pin must be low to read the flash contents. Flash WE is tied to bit 7.