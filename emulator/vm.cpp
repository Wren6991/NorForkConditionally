#include "vm.h"

void vm::step()
{
    CIR = *((instruction*)(memory + (PC & ~(0x07))));             // round to 8 byte boundaries.
    CIR.A = (CIR.A >> 8) | (CIR.A << 8);    // little endian reorder
    CIR.B = (CIR.B >> 8) | (CIR.B << 8);
    CIR.C = (CIR.C >> 8) | (CIR.C << 8);
    CIR.D = (CIR.D >> 8) | (CIR.D << 8);
    memory[CIR.A] = ~(memory[CIR.A] | memory[CIR.B]);
    PC = memory[CIR.A] ? CIR.C : CIR.D;
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
