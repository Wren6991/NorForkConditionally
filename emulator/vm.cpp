#include "vm.h"

void vm::step()
{
    CIR = *((instruction*)((&((*memory)[0])) + (PC & ~(0x07))));             // round to 8 byte boundaries.
    CIR.A = (CIR.A >> 8) | (CIR.A << 8);    // little endian reorder
    CIR.B = (CIR.B >> 8) | (CIR.B << 8);
    CIR.C = (CIR.C >> 8) | (CIR.C << 8);
    CIR.D = (CIR.D >> 8) | (CIR.D << 8);
    uint8_t result = ~(get(CIR.A) | get(CIR.B));
    write(CIR.A, result);
    PC = result ? CIR.C : CIR.D;
}

vm::vm()
{
    /*for (int i = 0; i < VM_MEM_SIZE; i++)
    {
        memory[i] = 0;
    }*/
    PC = 0;
    debug_written = false;
}

bool vm::isWriteable(uint16_t location)
{
    return (location >= 0x8000 && location < 0xc000) || location == DEBUG_OUT_POS;
}

inline bool isReadable(uint16_t location)   // certain locations do not yield any output, so default to 0.
{
    return location < 0xc000 || location == DEBUG_IN_POS;
}

uint8_t vm::get(uint16_t location)
{
    if (isReadable(location))
        return (*memory)[location];
    else
        return 0x00;
}

void vm::write(uint16_t location, uint8_t data)
{
    if (!isWriteable(location))
        return;
    (*memory)[location] = data;
    if (location == DEBUG_OUT_POS)  // or any other peripheral...
    {
        debug_written = true;
        for (unsigned int i = 0; i < peripherals.size(); i++)
            peripherals[i]->write(location, data);
    }
}
