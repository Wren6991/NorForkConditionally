Python Serial Programmer
========================

Consists of two parts:

- A C++ slave application running on an AVR, does the actual programming
- A Python host application running on a PC, serves up the data to the programming slave.

Protocol
--------

The slave application programs the EEPROM using its page mode, whereby 64 bytes are programmed in a single write cycle. These bytes are stored in the AVR's RAM after having been transferred over the serial connection, and are written to the EEPROM once the host gives the go-ahead. The AVR also remembers the current address this is written to, although a new address is supplied each time by the host. 

All communication is initiated by the host, beginning with the magic byte 'S' (0x53) followed by one of the following signals:

- P (page)
	- followed by 72 bytes: 64 bytes of page data, and 8 bytes of parity check data.
	- Each of the 64 parity bits matches the corresponding byte, with total odd parity.
	- slave returns "O" for OK, or "E" for error.
- A (address)
	- followed by 2 bytes, in big-endian order.
	- tells the AVR the next address to be written to.
	- "O" for OK, "E" for error.
- W (write)
	- tells the AVR to write the page and report back.
	- If the AVR fails to write the page (it may retry invisibly to the host), it sends an "O" back for OK; otherwise an "E".
	- guaranteed to finish within 100ms; otherwise the host resyncs.
- " " (space)
	- ignored by the AVR.
	- use to resync after an error, and to get UARTs in sync before sending data or commands (pad commands with ~4 preceding spaces!)

Initial State
-------------

On startup, the slave programmer device (AVR) has a defined initial state:

- All of the data pins (D0..7) are in hi-Z mode
- control pins E and F (wHADDR and wLADDR) are low.
- control pins G and H (#OE and #WE) are high.

This is to ensure minimal conflict with the computer system it's attached to.
Ensure that the clock line is held low before attaching the control pins; failure to do so can result in logic contention.