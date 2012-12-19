#include <iostream>
#include "tokenizer.h"
#include "parser.h"

#define _s(str) #str
#define _xs(str) _s(str)


int main()
{
    try
    {
        std::vector<token> tokens = tokenize(_s(
////////////////////////////////////////////////
const debugout 0xc000;
const debugin 0xc001;
function main()
{
    nfc(0xbfff, 0x0018);
    nfc(0xbfff, debugin);
    nfc(debugout, 0xbfff);
}

////////////////////////////////////////////////
        ));
    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
    }

}
