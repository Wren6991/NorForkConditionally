USB Programmer
==============

Hardware
--------

The computer is programmed with a USB programmer, connected to a desktop PC via USB, and to the data and control bus of the computer.
The programmer is actually an ATMEGA8-16PU with a USB connector and a 14-pin header connector, plus passives used for interface and decoupling.
The header is laid out as such:
```
+---+---+---+---+---+---+---+
|D7 |D6 |D5 |D4 |wHA|wLA|Vcc|
+---+---+---+---+---+---+---+
|D3 |D2 |D1 |D0 |/OE|/WE|GND|
+---+---+---+---+---+---+---+
```
Where D[0..7] are the data bus connections, wHA/wLA are the write signals for the high and low address latches, and /OE and /WE are the active low output/write signals for the EEPROM.
The programmer uses the address latches to control the 16-bit address bus via the 8-bit data bus - this means the address latches need to be enabled for the duration of programming, which is done by pulling the OE_ADDR line high on microcode 1, pin D6.
The clock line must be held low while programming, to avoid contention with the control unit over control signals.

The ATMEGA chip runs at 12MHz with a core voltage of 5V, as the EEPROM on the computer needs 5V to operate. The USB D+ and D- lines are clamped to 3.3V with Zener diodes.
