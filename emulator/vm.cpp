#include "vm.h"

void vm::step()
{
    CIR = *((instruction*)(memory + (PC & ~(0x07))));             // round to 8 byte boundaries.
    CIR.A = (CIR.A >> 8) | (CIR.A << 8);    // little endian reorder
    CIR.B = (CIR.B >> 8) | (CIR.B << 8);
    CIR.C = (CIR.C >> 8) | (CIR.C << 8);
    CIR.D = (CIR.D >> 8) | (CIR.D << 8);
    uint8_t result = ~(get(CIR.A) | get(CIR.B));
    if (isWriteable(CIR.A))
        memory[CIR.A] = result;
    PC = result ? CIR.C : CIR.D;
    if (CIR.A == DEBUG_OUT_POS)
        debugWritten = true;
}

vm::vm()
{
    for (int i = 0; i < VM_MEM_SIZE; i++)
    {
        memory[i] = 0;
    }
    PC = 0;
}

bool vm::isWriteable(uint16_t location)
{
    return (location >= 0x8000 && location < 0xc000) || location == 0xc000;
}

inline bool isReadable(uint16_t location)   // certain locations do not yield any output, so default to 0.
{
    return location < 0xc000 || location == 0xc001;
}

uint8_t vm::get(uint16_t location)
{
    if (isReadable(location))
        return memory[location];
    else
        return 0x00;
}
