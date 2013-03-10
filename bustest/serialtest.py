#! /usr/bin/env python

import serial
from time import sleep

global port

def sopen(name):
    """returns the serial port with the given name, or false if not found."""
    for i in range(256):
        try:
            s = serial.Serial(port = i, timeout = 0.2, baudrate = 9600)
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
    while len(buffer) < 64:
        buffer.append(0)
    if len(buffer) > 64:
        raise "Buffer is too long"
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
    port.write("    SW")

    

port = sopen("COM3")

print(port)


if port:
    sendaddress(0)
    sendpage(bytearray("abcdefghijklmnopqrstuvwxyz".encode()))
    response = ""
    b = port.read(1)
    while len(b) > 0:
        response += b.decode()
        b = port.read(1)
    print(response)
    port.close()


