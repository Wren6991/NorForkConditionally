#include <fstream>
#include <iomanip>
#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "printtree.h"
#include "compiler.h"
#include "linker.h"

#define _s(str) #str
#define _xs(str) _s(str)


void printout(std::vector<char> buffer, bool printasbytes = true)
{
    int nconsecutivezeroes = 0;
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        if (i % 8 == 0)
            std::cout << std::hex << std::setw(4) << std::setfill('0') << i << ":\t";
        std::cout <<  std::hex << std::setw(2) << std::setfill('0') << (((int)buffer[i]) & 0xff);
        if (i % 8 == 7)
            std::cout << "\n";
        else if (printasbytes || i % 2 == 1)
            std::cout << " ";

        if (buffer[i] == 0)
            nconsecutivezeroes++;
        else
            nconsecutivezeroes = 0;
        if (nconsecutivezeroes >= 8)
            break;
    }
}


int main()
{
    try
    {
        std::fstream sourcefile("./beep.spn", std::ios::in | std::ios::binary);
        if (!sourcefile.is_open())
        {
            throw(error("Error: could not open file!"));
        }
        sourcefile.seekg(0, std::ios::end);
        int sourcelength = sourcefile.tellg();
        sourcefile.seekg(0, std::ios::beg);
        std::vector<char> source(sourcelength);
        sourcefile.read(&source[0], sourcelength);
        source.push_back(0);
        std::cout << "Read:\n" << &source[0];
        std::vector<token> tokens = tokenize(&source[0]);
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
        return -1;
    }
    return 0;
}
