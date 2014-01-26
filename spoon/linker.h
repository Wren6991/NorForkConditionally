#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include "object.h"
#include "linkval.h"
#include "vardict.h"

#include <map>
#include <sstream>
#include <stdint.h>
#include <vector>

const int ROM_SIZE = 32 * 1024;
const int INCREMENT_START = ROM_SIZE - 1024;
const int DECREMENT_START = INCREMENT_START + 256;
const int LEFTSHIFT_START = DECREMENT_START + 256;
const int RIGHTSHIFT_START = LEFTSHIFT_START + 256;
const int RAM_SIZE = 16 * 1024;
const int RAM_TOP = 0xbfff;
const int HEAP_TOP = 0xbfbf;
const int HEAP_BOTTOM = 0x8000;
const int HEAP_SIZE = HEAP_TOP - HEAP_BOTTOM + 1;

const int POINTER_READ_INSTRUCTION = RAM_TOP - 0x2f;
const int POINTER_READ_PVECTOR = POINTER_READ_INSTRUCTION + 0x0a;       // B field of the next instruction - the one we read from.
const int JUMP_INSTRUCTION = RAM_TOP - 0x1f;
const int JUMP_PVECTOR = JUMP_INSTRUCTION + 0x6;
const int POINTER_READ_RESULT = RAM_TOP - 0x0f;
const int POINTER_WRITE_VALUE = POINTER_READ_RESULT;
const int POINTER_WRITE_CLEAR_INSTRUCTION = POINTER_READ_INSTRUCTION - 0x10;
const int POINTER_WRITE_COPY_INSTRUCTION = POINTER_WRITE_CLEAR_INSTRUCTION + 0x8;

class linker
{
    vardict vars;
    std::map<std::string, definition*> defined_funcs;
    std::vector<definition*> definitions;
    int index;
    int current_address;
    bool compile_to_ram;
    std::vector<linkval> buffer;
    std::map<std::string, linkval> valtable;        // contains values for substitution
    std::vector<std::pair<std::string, std::string> > stringvalues;   // label, contents
    std::stringstream defstring;    // contains global variable and function export data
    void savelabel(std::string, linkval);
    void write8(linkval);
    void write16(linkval);
    void padto8bytes();
    linkval evaluate_or_return_literal(expression*);
// Code generation routines:
    void emit_nfc2(linkval x, linkval y);
    void emit_branchifzero(linkval testloc, linkval dest, bool amend_previous = false, bool invert = false);
    void emit_branchifnonzero(linkval testloc, linkval dest, bool amend_previous = false);           // shim for e_biz with invert = true
    void emit_branchalways(linkval dest, bool always_emit = false);     // can pass true to disable the follow behaviour, if the jump is jumped to (e.g. end of while)
    void emit_copy(linkval src, linkval dest);
    void emit_copy_inverted(linkval src, linkval dest);
    void emit_writeconst(uint8_t val, linkval dest);
    void emit_copy_multiple(linkval src, linkval dest, int nbytes);
    void emit_writeconst_multiple(int value, linkval dest, int nbytes);
    void emit_writelabel(std::string label, linkval dest);
    bool last_instruction_points_to_this_one();
// Tree traversal:
    void link(funcdef*);
    void exportfuncdef(funcdef*);
    void exportvardeclaration(vardeclaration *vardec);
    void link(block*);
    void link(statement*);
    void link(funccall*);
    linkval linkfunctioncall(std::vector<expression*>&, funcdef*);
    linkval linkbuiltinfunction(std::vector<expression*>&, std::string, bool givenpreferred = false, linkval preferred = 0);
    void link(goto_stat*);
    void link(if_stat*);
    void link(while_stat*);
    void link(assignment*);
    linkval evaluate(expression*, bool givenpreferred = false, linkval preferred = 0);
    uint16_t evaluate(linkval);
    std::vector<char> assemble();
    void allocatefunctionstorage();
    std::set<std::string> analysedependencies(std::string rootfunc);
    void removeunusedfunctions();
public:
    linker();
    void add_object(object* obj);
    std::vector<char> link();
    std::string getdefstring();
    void setcompiletoram(bool ram);
    bool strip_unused_functions;
};

#endif // LINKER_H_INCLUDED
