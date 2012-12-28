#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "printtree.h"

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
function main()
{
    nfc(0xbfff, 0x0018);
    nfc(0xbfff, debugin);
    nfc(debugout, 0xbfff, 0x0000, 0x0000);
}

////////////////////////////////////////////////
        ));
        parser p(tokens);
        printtree(p.getprogram());
    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
    }

}
