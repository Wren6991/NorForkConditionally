#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "printtree.h"
#include "compiler.h"
#include "linker.h"

#define _s(str) #str
#define _xs(str) _s(str)


int main()
{
    try
    {
        std::vector<token> tokens = tokenize(_s(
////////////////////////////////////////////////
const pointer debugout = 0xc000;
const pointer debugin = 0xc001;

function test();

function main()
{
    nfc(0xbfff, 0x0018);
    nfc(0xbfff, debugin);
    nfc4(debugout, 0xbfff, 0x0000, 0x0000);
    test();
}

function test()
{
}

function nfc(pointer a, pointer b)
{
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
        l.link();
    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
    }

}
