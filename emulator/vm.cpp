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
    return location >= 0x8000 && location < 0xc008;
}

uint8_t vm::get(uint16_t location)
{
    if (location >= 0xc000 && location < 0xc008 && location != DEBUG_IN_POS)
    {
        for (unsigned i = 0; i < peripherals.size(); i++)
            if (peripherals[i]->readlocations.find(location) != peripherals[i]->readlocations.end())
                return peripherals[i]->read(location);
        return 0;
    }
    else
    {
        return (*memory)[location];
    }
}

void vm::write(uint16_t location, uint8_t data)
{
    if (!isWriteable(location))
        return;
    (*memory)[location] = data;
    if (location >= DEBUG_OUT_POS && location < 0xc008)
    {
        if (location == DEBUG_OUT_POS)
            debug_written = true;
        for (unsigned int i = 0; i < peripherals.size(); i++)
            peripherals[i]->write(location, data);
    }
}
