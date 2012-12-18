#include <iostream>
#include "tokenizer.h"

#define _s(str) #str
#define _xs(str) _s(str)

using namespace std;

int main()
{
    try
    {
        std::vector<token> tokens = tokenize(_s(
////////////////////////////////////////////////
def debugout c000           \n
def debugin c001            \n

def clear(address)          \n
  address 'ff               \n
end \n

def invcopy(src, dest)      \n
  clear(dest)               \n
  dest src                  \n
end                         \n

def not(address)            \n
  address address           \n
end                         \n
////////////////////////////////////////////////
        ));
    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
    }

}
