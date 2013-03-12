#! /usr/bin/env python

import serial
from time import sleep

global port

class ProgrammerError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

def sopen(name):
    """returns the serial port with the given name, or false if not found."""
    for i in range(256):
        try:
            s = serial.Serial(port = i, timeout = 0.1, baudrate = 9600)
            if (s.portstr == name):
                return s
            else:
                s.close()
        except serial.SerialException:
            pass
    return False

def sendaddress(address):
    command = bytearray("    SA".encode())
    command.append((address >> 8) & 0xff)
    command.append(address & 0xff)
    port.write(command)


def sendpage(buffer):
    newbuffer = bytearray([x for x in buffer])
    buffer = newbuffer
    while len(buffer) < 64:
        buffer.append(0)
    if len(buffer) > 64:
        raise ProgrammerError("Buffer is too long: " + str(len(buffer)))
    parity = 0
    for i in range(64):
        if i % 8 == 0:
            parity = 0
        bit = (buffer[i] >> 4) ^ (buffer[i] & 0xf)
        bit = (bit >> 2) ^ (bit & 0x3)
        bit = (bit >> 1) ^ (bit & 0x1)
        parity = (parity << 1) | bit
        if i % 8 == 7:
            buffer.append((~parity) & 0xff)  # odd parity, so we complement
    port.write(bytearray("    SP".encode()) + buffer)

def sendwrite():
    port.write("    SW".encode())

def dountilsuccess(func):
    """Repeatedly calls func until it receives "O"; returns true if a timeout occurred while doing so."""
    port.flushInput()
    hastimedout = False
    while True:
        func()
        response = port.read(1).decode()
        print(response)
        if len(response) != 1:
            hastimedout = True
        if response == "O":
            return hastimedout


def programpage(address, buffer):
    failurecount = 0
    while True:
        dountilsuccess(lambda: sendaddress(address))
        if dountilsuccess(lambda: sendpage(buffer)):
            pass    # resend address if a timeout occurred
        port.flushInput()
        sendwrite()
        if port.read(1).decode() == "O":
            break
        else:
            failurecount += 1
            if failurecount > 5:
                raise ProgrammerException("Failed to write page at address " + str(address))

def programfile(filename):
    with open(filename, "rb") as f:
        address = 0
        while True:
            page = f.read(64)
            if len(page) <= 0:
                break
            programpage(address, bytearray(page))
            address += 64



port = sopen("COM3")

print(port)


if port:
    programpage(0, bytearray([
        0xbf, 0xff, 0x00, 0x18, 0x00, 0x08, 0x00, 0x08,
        0xbf, 0xff, 0xc0, 0x01, 0x00, 0x10, 0x00, 0x10,
        0xc0, 0x00, 0xbf, 0xff, 0x00, 0x00, 0x00, 0x00,
        0xff
        ]))
    response = ""
    b = port.read(1)
    while len(b) > 0:
        response += b.decode()
        b = port.read(1)
    print(response)
    port.close()


