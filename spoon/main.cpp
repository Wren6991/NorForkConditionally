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
        std::fstream sourcefile("./test.spn", std::ios::in);
        sourcefile.seekg(0, std::ios::end);
        int sourcelength = sourcefile.tellg();
        sourcefile.seekg(0, std::ios::beg);
        char *source = new char[sourcelength + 1];
        source[sourcelength] = 0;
        sourcefile.read(source, sourcelength);
        std::cout << "Read:\n" << source;
        std::vector<token> tokens = tokenize(source);
        parser p(tokens);
        program *prog = p.getprogram();
        printtree(prog);
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
