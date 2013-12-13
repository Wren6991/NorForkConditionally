Fork Machine: OISC Processor
==============

<img src="processor.png">

Contents:

- [Machine](#machine)
- [Execution](#execution)
- [Microcode Format](#microcode-format)
- [Breadboard Layout](#breadboard-layout)
- [Control Unit](#control-unit)
- [Memory Board](#memory-board)
- [PC/ADDR Board](#pcaddr-board)
- [ALU Board](#alu-board)
- [IO Board](#io-board)
- [Flash Board](#flash-board)


Machine
-------

I built an 8-bit CPU on 6 breadboards, with around 30 chips - mostly simple CMOS logic chips, plus a few other components like RAM. I'm still working on the software side of things (check out the samples for my compiler in the spoon directory!) but the hardware is essentially in a finished state.

Hardware features:

- Custom CPU, with only one instruction (but that's *plenty*)
- Clockspeed of 8MHz
- 32KiB of ROM for system calls and the bootstrap program
- 16KiB of RAM for data and programs
- More input and output than you can shake a stick at!
- Half a megabyte (!) of flash storage for programs and documents

A 20x4 character LCD provides text output for a simple console interface - a keyboard is the only thing that I still intend to add. 

Execution
----------

The machine has a clock frequency of 8MHz, or eight million operations per second - however, the execution of a single instruction takes 10 steps, yielding a final throughput of 0.8 MIPS.

The steps are as follows:

1.  Fetch bytes (we fetch the first byte last, so its location is still in the latch for the writeback)
	-  `^A` <= (`PC` & ~0x7 | 0x2)
	-  `_A` <= (`PC` & ~0x7 | 0x3)
	-  `Y` <= (`A`)
	-  `^A` <= (`PC` & ~0x7 | 0x0)
	-  `_A` <= (`PC` & ~0x7 | 0x1)
	-  `X` <= (`A`)
2.  NOR them, store in first location
	- (`A`) <= `[ALU]`
3.  Conditionally branch to 3rd or 4th address
	- `^A` <= (`PC` & ~0b111 | 0b0Z0)
	- `_A` <= (`PC` & ~0b111 | 0b0Z1)
	- `PC` <= `A`

`A` (the address latch) is a 16-bit register but is loaded 8 bits at a time via the data bus, meaning it takes 2 clocks to load it from memory.

Giving the machine a symmetrical 16-bit data bus and memory to match the address bus would therefore reduce clocks per instruction (MHz/MIPS) by 30 percent! It would also make the wiring much more error-prone, and speed really wasn't the goal here. Performance-wise this would be polishing the turd.

The control unit can modify the last 3 bits of an address to load the appropriate byte of an 8-byte instruction (I showed this above with the C-style bit operations).

Microcode Format
----------------

The microcode (ucode) format is entirely horizontal - i.e. no encoding. The initial design was 20 clk/instruction and had a fairly encoded microcode format, but I've made things faster and simpler.

The address is 5 bits long, and each microcode word is 16 bits long:

`ZAAAA -> oooossss _wwwwwwr`

The first address bit, Z, comes from the ALU via the control bus and is 1 if the result of the last ALU operation was 0x00 (else, 0).
Bits 0-3 of the address are from the microcode address counter. Each of the 32 addresses maps to a 16-bit control word, which is routed out via the control bus to control the CPU.

Bit assignment for the microcode word:

```
+-----+-----------------+
| Bit | uROM 1 | uROM 0 |
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
```
  
Binary listing: oooossss _wwwwwwr

```
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
```

Z indicates that a bit should be 1 when Z_DETECT is 1, and 0 when 0. Yes, 2 bits different between the two microcode paths, it seems wasteful, but in terms of the need for external logic and the speed it's actually quite efficient to just stick everything in the uROM (see ROM-based ALUs etc.).

Same listing in hex:
```
6a10, 6b08, a002, 6810, 6908, a004, 9020, 6c10, 6d08, 8040, 0001
Or, if Z:             ''                  6e10, 6f10,     ''
```

I ended up programming the uROMs by hand, although a simple Arduino sketch wouldn't have been much effort. By the time I got to the breadboard stage the design was fairly mature, so I thankfully haven't had to change them!

Breadboard Layout:
------------------

```
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
```
Components:
```
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
```

Most of the latches are arranged "notch down" (I turned the computer round halfway through making it... errr...) with pins on the databus in the following order:

```
    +------+
Q4 -|      |- Q3
    |      |
Q5 -|      |- Q2
    |      |
Q6 -|      |- Q1
    |      |
Q7 -|      |- Q0
    +--/\--+

Ds are between Qs: QDDQQDDQ
                   00112233
```

The nor gates are arranged differently:

```
    +------+
 0 -|      |- 3
    |      |
 1 -|      |- 2
    +--/\--+

    +------+
 4 -|      |- 7
    |      |
 5 -|      |- 6
    +--/\--+

Data sheet pin order...
```

Control Unit
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

Microcode address signals uA1 and uA3 (values 2, 8) are connected to a diode-resistor AND gate:

```
    Vcc
    |  10k resistor
   [ ]
   [ ]
    |
    |--|>|-- uA1    (  -|>|-  is a schottky diode  )
rst-|
    |--|>|-- uA3
```

Which is then connected to the counter's reset pin. The address changes while the clock is low, meaning the counter is reset during the quiescent "tock" phase.

Previously there was an 11th ucode word that had a reset signal, but this causes two problems: If the last word is all 0s + a reset signal, part of the first ucycle will be spent on the reset operation, increasing that cycle's minimum timespan. As all cycles are the same length, this new critical path will slow down the entire CPU! If the last word is equal to the first word + a reset signal, you can avoid this problem - however, you still have the problem that any bit skew of the microcode address as it changes to 0 then the wrong word will be momentarily asserted, which can cause all sorts of fun bugs.

The only feedback from the rest of the computer to the control unit is the Z_DETECT line from the ALU - this feeds into the top bit of the microcode address, causing two different sequences of operations to be performed based on the result of the last ALU operation.

The second half of the counter chip (two four-bit counters on a chip) divides the 8MHz clock down to 4->2->1->0.5 MHz, which can be selected with a short jumper wire, although the system is now (fingers crossed) stable at 8MHz provided a reasonable power supply.

Memory Board
------------

Not much to see here!

The ROM and RAM chip are connected to the data bus and the address bus - 15 bits of the address bus go to the ROM (32KiB) and 14 bits to the RAM (16KiB). ROM, RAM and IO devices are memory mapped in a single 16-bit address space.

ROM contains the bootstrap program and code for all the library/system calls used by user programs. RAM stores data, and one user program at a time.

The top 2 bits of the address bus go to 1->4 demultiplexers, which are activated by the general OE/WE signals, and select devices with a resolution of 16KiB (four segments).

The ROM is not writable (at least not by software!) so its #WE pin is held high by a 100k pullup (see the document on programming). WE for segments 2 and 3 (`8000`->`bfff`, `c000`->`ffff`) are routed to RAM and the IO board, respectively.

ROM spans two segments, so another schottky diode-resistor AND gate is used as a negative logic OR gate to smush the two #OE signals together. The diodes give a nice sharp falling edge, and the 10k pullup gives a slightly less sharp (few nanoseconds) rising edge.

The top segment signals (`c000`->`ffff`) are routed to the IO board - read on for details.

PC/ADDR Board
-------------

This board contains the program counter, which is a single 16-bit register containing the address of the current instruction, and the address latch, which has 2 8-bit inputs and one 16-bit output, and addresses a byte to be read from memory.

The address latches load from the data bus, and output to something I call the immediate address bus (I need a better name!). This bus is internal to the PC/ADDR board.

The program counter loads from and outputs to the immediate address bus - during an instruction cycle it is loaded in a single 16-bit transfer from the address latch.


```
                        |OE
       +-------------+  v   |           |CONTROL     |
       |     PC      |<---->|           v            |
       +-------------+      |     +------------+     |
                        |OE |---->|SUFFIX LOGIC|---->|
       +-------------+  v   |     +------------+     |
data-->|ADDRESS LATCH|----->|                        |
       +-------------+      |                        |
         premodified address^             address bus^
```


There is only one other chip on this board, and this chip accounts for the need for the immediate address bus: a multiplexer, called the suffix selector, selects the source of the least significant 3 bits of the outside address bus.

Why is this needed? When reading/writing to the location of the address latch, the suffix selector receives select signal 0: the immediate address provides the last 3 bits. In other words, the address of the RAM/ROM location, IO device or whatever is the same as the program counter / address latch. It's a non-operation.

When the select signal is 1, the *microcode* provides the last 3 bits of the address. This is important, because the instructions are 8 (2 ** 3) instructions long.

As a result, the microcode can select which of the 8 bytes that form the instruction it would like to address, relative to the current PC value.

The more signicant 13 bits of the IAB are connected directly to the address bus.

The address bus doesn't have any pulldowns, as it's driven every clock cycle.

ALU Board
---------

The ALU is very simple - it provides only one operation, bitwise NOR. The other chips store operands, interface with the data bus, and inform the control unit as to whether the last result was a zero.

- `X`, `Y`: these are the 8-bit operand registers. They are loaded from the data bus, and output directly to the NOR gates
- `NOR0-3`, `NOR4-7`: NOR gates that implement the ALU's only function. Their outputs go to the data buffer and the zero-detect logic
- `ALUBUFF`: an 8-bit buffer that asserts the result of the last operation on the data bus
- `NORall`: an 8-input NOR gate; when all its inputs are 0, its result is a one. Otherwise, it's a zero. Feeds back to the control unit

The control unit controls the WE and #OE signals directly (i.e. horizontal microcode).
The speed of the NOR gates themselves is actually not very important, as the operands are present by the end of the preceding cycle to the result being used, and the NOR operation is completely combinational.

```
|  +---+              +---------------+
|->| X |   +-----\    |               v
|  +---+-->\      \   |  +-------+ +-------+
|           > NOR  )o-+->|ALUBUFF| |NOR_all|
|  +---+-->/      /      +-------+ +-------+
|->| Y |   +-----/           |         |
|  +---+                     v         +-> Z_DETECT 
+--------------------------------+
  data bus                       |
```
*ASCII diagrams like it's 1979*


IO Board
--------

Again, not much to see.

The board mostly consists of memory-mapped latches which are written to and read from by software - the memory mapping is performed by a pair of 1->8 demultiplexers.

The #OE dmux is a 74137, the inverting type, while the WE dmux is a 74237, the non-inverting type (See the 74373 datasheet: the #OE/LE polarities are different). Previously I borrowed part of a nearby 7404 to invert the signal, but now that there is more than one output I thought I may as well buy the correct chip and take that link out of the timing chain (I never expected it to work at all with that inverter there). It never pays to bodge! Except when it does.

It's of vital importance that the output latches read as all 0s, else the write will be corrupted; as they do not drive the data bus low when they are read (they can't, their outputs are pointed at the outside world) it wouldn't be unthinkable for spurious data to be left on the data bus at higher clock speeds, which would completely mess up the write operation.

It's for this reason that the data bus is pulled down rather aggressively, with 4k7 resistors.

The terms *input* and *output* are used relative to the computer, i.e. *out* denotes databus->peripheral, and *in* peripheral->databus:

- `DBGOUT` - output, address `c000`
    - Connected to 8 LEDs, and bits 7, 6, 3, 2, 1, 0 are connected to the LCD.
- `DBGIN` - input, address `c001`
    - Connected to an 8 bit dip switch. 100k resistors are more than sufficient for the pulldown, as human input will never exceed a few Hz.
- `SIG` - output, address `c002`
    - Ought really to be on the flash board but there wasn't quite space. There used to be a piezo transducer in this position, I need to find a smaller one to squeeze in.
    - Bits 2->0 form the most significant 3 bits of the flash address (512KiB so 19 bits)
    - Bit 7 is flash WE
    - Bit 6 is #OE for the flash chip; it is also inverted by an NPN transistor to give #OE for the flash input register (0: flash->`OUT`, 1: `IN`->flash.) They're complementary to avoid contention. More of that later!

Flash Board
-----------

This board contains a half-megabyte A29040B (soon to be SST39SF040, for 4k sectors instead of 64k) flash chip, and the input and output registers to control it.

Hardware-wise there's nothing particularly complicated going on - it's the software that works the magic on this board.

- `MID` - output, address `c003`
	- bits 15-8 of the flash address
- `LO` - output, address `c004`
	- bits 7-0 of the flash address
- `IN` - output, address `c005`
	- I know what I said about data direction, but in this case "in" is relative to the flash chip.
	- Asserts its value on the flash IO pins when bit 6 of the SIG register is *high*. (`#OE <= not SIG(6)`)
- `OUT` - input, address `c006`
 	- Asserts flash IO pins onto data bus when read.
	- Its LE pin is tied high so it's really a buffer rather than a register, but it was convenient to use a 74373 and keep the same pinout across the board.

The flash chip's #OE is tied to bit 6 of the SIG register - this pin must be low to read the flash contents. Flash WE is tied to bit 7.

With some moderately fancy software, data can be stored or read from the flash chip pretty much arbitrarily - this provides storage for users' documents and programs.