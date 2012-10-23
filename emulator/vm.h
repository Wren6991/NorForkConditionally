#ifndef _VM_H_INCLUDED_
#define _VM_H_INCLUDED_

#include <stdint.h>

struct instruction
{
    uint16_t A, B, C, D;
};

struct vm
{
    uint8_t memory[0x4000];
    uint16_t PC;
    bool debugWritten;      // so the user can check if there has been debug output.
    instruction CIR;
    void step();
    vm();
};

#endif // _VM_H_INCLUDED_
