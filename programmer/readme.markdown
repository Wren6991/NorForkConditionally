USB Programmer
==============

<img src="http://i.imgur.com/08vBQ4q.jpg">

Hardware
--------

The computer is programmed with a USB programmer, connected to a desktop PC via USB, and to the data and control bus of the computer.
The programmer is actually an ATMEGA8-16PU with a USB connector and a 14-pin header connector, plus passives used for interface and decoupling.
The (male) header is laid out as such:
```
+---+---+---+---+---+---+---+
|D7 |D6 |D5 |D4 |wHA|wLA|Vcc|
+---+---+---+---+---+---+---+    A
|D3 |D2 |D1 |D0 |/OE|/WE|GND|    | Up (notch)
+---+---+---+---+---+---+---+
```
Where D[0..7] are the data bus connections, wHA/wLA are the write signals for the high and low address latches, and /OE and /WE are the active low output/write signals for the EEPROM.
The programmer uses the address latches to control the 16-bit address bus via the 8-bit data bus - this means the address latches need to be enabled for the duration of programming, which is done by pulling the OE_ADDR line high on microcode 1, pin D6.
The clock line must be held low while programming, to avoid contention with the control unit over control signals.

The ATMEGA chip runs at 12MHz with a core voltage of 5V, as the EEPROM on the computer needs 5V to operate. The USB D+ and D- lines are clamped to 3.3V with Zener diodes.

Software
--------

The ATMEGA is running the V-USB soft USB stack (www.obdev.at/vusb/) for its USB communications - data is streamed from the computer over USB one page (64 bytes) at a time, and the programmer writes each page to the EEPROM, verifying that it is correctly written before returning a success code.
The programming cycle goes as follows:

- Receive page
- Write high address byte to data bus, pulse wHA high.
- Write low address byte to data bus, pulse lHA high.
- Write first data byte to data bus, pulse /WE low.
	- repeat for up to 63 more bytes
- Wait for 1ms, or until the EEPROM polls as ready.
- Verify data.
- If successful, ask computer for next page of data. Else retry.

On the PC side, there is a simple command-line tool which reads a *.bin file (name passed as a command-line argument) and streams it page-by-page to the programmer. USB control is done with libusb, or its Windows counterpart.

Protocol
--------

A simple protocol is defined for two-way communication between the host PC and the programmer. The first byte defines the type of instruction, and it is optionally followed by n (< 255) bytes of data, as required by the instruction.

- Opcode 0x0: `NULL`
	- Does absolutely nothing
	- As it's one byte long, you can resync the connection by sending NULLs until you get an ACK.
- Opcode 0x1: `ACK`
	- Acknowledge.
- Opcode 0x2: `NAK`
	- Negative acknowledge
	- "What?"
- Opcode 0x3: `RTS`
	- Request to send
	- Sent by host before sending page.
	- Response is `ACK`.
- Opcode 0x4: `DAT`
	- Sending data
	- followed by single-byte size == n, and then n bytes of data.
	- e.g. `0x0403abcdef` would send 3 bytes of data: `ab`, `cd` and `ef`.
- Opcode 0x5: `NXT`
	- Programmer to host: next page please.
	- Host replies with an `RTS`, which the programmer should `ACK`.
- Opcode 0x6: `ERR`
	- Programmer has encountered an error
	- followed by size byte, then a null-terminated error string. (Double whammy...)
