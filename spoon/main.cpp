#include <iomanip>
#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "printtree.h"
#include "compiler.h"
#include "linker.h"

#define _s(str) #str
#define _xs(str) _s(str)

void printout(std::vector<char> buffer)
{
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (((int)buffer[i]) & 0xff);
        if (i % 8 == 7)
            std::cout << "\n";
        else if (i % 2 == 1)
            std::cout << " ";
    }
}


int main()
{
    try
    {
        std::vector<token> tokens = tokenize(_s(
////////////////////////////////////////////////
const pointer debugout = 0xc000;
const pointer debugin = 0xc001;

function main()
{
    nfc(0xbfff, 0x0018);
    nfc(0xbfff, debugin);
    nfc4(debugout, 0xbfff, 0x0000, 0x0000);
}


////////////////////////////////////////////////
        ));
        parser p(tokens);
        program *prog = p.getprogram();
        //printtree(prog);
        compiler c;
        object *obj = c.compile(prog);
        std::cout << "\nPost-compile tree:\n\n";
        printtree(prog);
        std::cout << "Linking...\n\n";
        linker l;
        l.add_object(obj);
        std::vector<char> machinecode = l.link();
        printout(machinecode);

    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
    }

}
