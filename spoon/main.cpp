#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd
    #define FILE_SEP_CHAR "\\"
#else
    #include <unistd.h>
    #define FILE_SEP_CHAR "/"
#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "printtree.h"
#include "compiler.h"
#include "linker.h"


void printout(std::vector<char> buffer, bool printasbytes = true)
{
    int nconsecutivezeroes = 0;
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        if (i % 8 == 0)
            std::cout << std::hex << std::setw(4) << std::setfill('0') << i << ":\t";
        std::cout << "0x" <<  std::hex << std::setw(2) << std::setfill('0') << (((int)buffer[i]) & 0xff);
        if (i % 8 == 7)
            std::cout << ",\n";
        else if (printasbytes || i % 2 == 1)
            std::cout << ", ";

        if (buffer[i] == 0)
            nconsecutivezeroes++;
        else
            nconsecutivezeroes = 0;
        if (nconsecutivezeroes >= 8)
            break;
    }
}

int imax(int a, int b)
{
    return a > b ? a : b;
}

int main(int argc, char **argv)
{
    std::string usage = "Usage: spoon [-s] (inputfile) (outputfile)\n";
    std::string ifilename, ofilename;
    bool have_ifilename = false, have_ofilename = false;
    bool strip_unused_functions = false;
    bool compile_to_ram = false;
    bool export_symbols = false;
    // Don't know why but it doesn't work on Linux without this code...
    /*for (int i = 1; i < argc; i++)
        argv[i][0] = 1 + (--argv[i][0]); // it's a no-op*/
    try
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                switch (argv[i][1])
                {
                case 's':
                    #ifdef EBUG
                    std::cout << "received option strip\n";
                    #endif
                    strip_unused_functions = true;
                    break;
                case 'r':
                    #ifdef EBUG
                    std::cout << "received option compile-to-ram\n";
                    #endif
                    compile_to_ram = true;
                    break;
                case 'e':
                    #ifdef EBUG
                    std::cout << "Received option export\n";
                    #endif // EBUG
                    export_symbols = true;
                    break;
                default:
                    throw(error(usage));
                }
            }
            else
            {
                if (!have_ifilename)
                {
                    ifilename = argv[i];
                    have_ifilename = true;
                }
                else if (!have_ofilename)
                {
                    ofilename = argv[i];
                    have_ofilename = true;
                }
                else
                {
                    throw(error(usage));
                }
            }
        }
        if (!(have_ifilename && have_ofilename))
            throw(error(usage));
        std::string ifiledirectory;
        if (ifilename.substr(0, 1) == "/" || ifilename.substr(1, 1) == ":")
            ifiledirectory = ifilename.substr(0, imax(ifilename.rfind("/"), ifilename.rfind("\\"))) + FILE_SEP_CHAR;
        else
        {
            char *buffer = new char[1024];
            getcwd(buffer, 1024);
            ifiledirectory = std::string(buffer) + FILE_SEP_CHAR + ifilename.substr(0, imax(ifilename.rfind("/"), ifilename.rfind("\\"))) + FILE_SEP_CHAR;
            delete buffer;
        }
        //std::cout << "Working directory: " << ifiledirectory << "\n";
        std::fstream sourcefile(ifilename, std::ios::in | std::ios::binary);
        if (!sourcefile.is_open())
        {
            throw(error(std::string("Error: could not open file \"") + ifilename + "\""));
        }
        sourcefile.seekg(0, std::ios::end);
        int sourcelength = sourcefile.tellg();
        sourcefile.seekg(0, std::ios::beg);
        std::vector<char> source(sourcelength);
        sourcefile.read(&source[0], sourcelength);
        source.push_back(0);
        sourcefile.close();
        std::vector<token> tokens = tokenize(&source[0]);
        parser p(tokens, ifilename, ifiledirectory);
        program *prog = p.getprogram();
        //printtree(prog);
        compiler c;
        object *obj = c.compile(prog);
#ifdef EBUG
        std::cout << "\n\nPost-compile tree:\n\n";
        //printtree(obj->tree);
#endif // EBUG
        linker l;
        l.strip_unused_functions = strip_unused_functions;
        l.setcompiletoram(compile_to_ram);
        l.add_object(obj);
        std::vector<char> machinecode = l.link();
#ifdef EBUG
        //printout(machinecode);
#endif
        std::fstream outfile(ofilename, std::ios::out | std::ios::binary);
        if (!outfile.is_open())
            throw(error(std::string("Error: could not open file ") + ofilename));
        for (unsigned int i = 0; i < machinecode.size(); i++)
            outfile.put(machinecode[i]);
        outfile.close();
        if (export_symbols)
        {
            std::fstream deffile(ofilename + ".def", std::ios::out | std::ios::binary);
            deffile << l.getdefstring();
            deffile.close();
        }
    }
    catch (error e)
    {
        std::cout << e.errstring << "\n";
        return 1;
    }
    return 0;
}
