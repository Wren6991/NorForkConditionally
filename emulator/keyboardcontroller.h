#ifndef KEYBOARDCONTROLLER_H_INCLUDED
#define KEYBOARDCONTROLLER_H_INCLUDED

#include "peripheral.h"

class KeyboardController: public peripheral
{
public:
    uint8_t keycode;
    KeyboardController(): keycode(0) {readlocations.insert(0xc007);}
    virtual void write(unsigned int address, unsigned int value) {}
    virtual unsigned int read(unsigned int address) {uint8_t val = keycode; keycode = 0; return val;}
};

#endif
