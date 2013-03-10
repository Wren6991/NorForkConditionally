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
        std::cout << "" <<  std::hex << std::setw(2) << std::setfill('0') << (((int)buffer[i]) & 0xff);
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


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: spoon (inputfile) (outputfile)\n";
    }
    else try
    {
        std::fstream sourcefile(argv[1], std::ios::in | std::ios::binary);
        if (!sourcefile.is_open())
        {
            throw(error(std::string("Error: could not open file ") + argv[1]));
        }
        sourcefile.seekg(0, std::ios::end);
        int sourcelength = sourcefile.tellg();
        sourcefile.seekg(0, std::ios::beg);
        std::vector<char> source(sourcelength);
        sourcefile.read(&source[0], sourcelength);
        source.push_back(0);
        sourcefile.close();
        std::vector<token> tokens = tokenize(&source[0]);
        parser p(tokens);
        program *prog = p.getprogram();
        compiler c;
        object *obj = c.compile(prog);
#ifdef EBUG
        printtree(obj->tree);
#endif // EBUG
        linker l;
        l.add_object(obj);
        std::vector<char> machinecode = l.link();
#ifdef EBUG
        printout(machinecode);
#endif
        std::fstream outfile(argv[2], std::ios::out | std::ios::binary);
        if (!outfile.is_open())
            throw(error(std::string("Error: could not open file ") + argv[2]));
        for (unsigned int i = 0; i < machinecode.size(); i++)
            outfile.put(machinecode[i]);
        outfile.close();
    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
        return -1;
    }
    return 0;
}
