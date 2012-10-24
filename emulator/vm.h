#ifndef _VM_H_INCLUDED_
#define _VM_H_INCLUDED_

#include <stdint.h>

const int VM_MEM_SIZE = 0x10000;
const uint16_t DEBUG_OUT_POS = 0xc000;
const uint16_t DEBUG_IN_POS = 0xc001;

struct instruction
{
    uint16_t A, B, C, D;
};

struct vm
{
    uint8_t memory[VM_MEM_SIZE];
    uint16_t PC;
    bool debugWritten;      // so the user can check if there has been debug output.
    instruction CIR;
    void step();
    bool isWriteable(uint16_t location);
    uint8_t get(uint16_t location);
    vm();
};

#endif // _VM_H_INCLUDED_
