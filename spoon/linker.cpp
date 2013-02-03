#include "error.h"
#include "linker.h"

#include <set>
#include <sstream>

#include <iostream>


std::string getlabel()
{
    static int lcount = 0;
    std::stringstream ss;
    ss << "__L" << std::hex << lcount++;
    return ss.str();
}

uint16_t getconstaddress(uint8_t val)
{
    return DECREMENT_START + ((val + 1) % 256);
}

//////////////////////// Vardict defs /////////////////////////////

void vardict::find_first_available_space(int searchstart)
{
    for (int i = searchstart; i < HEAP_SIZE; i++)
    {
        if (!memory[i])
        {
            first_available_space = i;
            return;
        }
    }
    // If it fell through:
    throw(error("Error: no more free space in heap!"));
}

// Search through memory for a block large enough
// If we find it, block it out and return the start.
int vardict::getspace(int size)
{
    int start = first_available_space;
    while (start < HEAP_SIZE)
    {
        bool enoughSpace = true;
        for (int i = start; i < start + size; i++)
        {
            if (memory[i])
            {
                enoughSpace = false;
                start = i + 1;          // start checking again at the next unchecked location
                break;
            }
        }
        if (enoughSpace)
        {
            for (int i = start; i < start + size; i++)
                memory[i] = true;
            if (start == first_available_space)
                find_first_available_space(first_available_space);       // if we've covered the first known free space, find a new one.
            return start;
        }
    }
    throw(error("Error: no more free space in heap!"));
}

// Note: returns an offset from the heap start, you'll have to add
// the heap bottom to this value to get a machine address.
int vardict::addvar(std::string name, type_enum type)
{
    variable *var = new variable;
    var->type = type;
    var->offset = getspace(typesizes[type]);
    if (vars.find(name) != vars.end())
        var->next = vars[name];  // push the stack down one...
    vars[name] = var;
    return var->offset;
}

void vardict::remove(std::string name)
{
    // sanity check:
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        throw(error("Error: tried to free non-existent variable! (Link-time)"));
    // release the memory:
    for (int i = iter->second->offset; i < iter->second->offset + typesizes[iter->second->type]; i++)
        memory[i] = 0;
    if (iter->second->offset < first_available_space)
        first_available_space = iter->second->offset;
    // update the dictionary: pop or remove
    if (iter->second->next)
    {
        variable *old = iter->second;
        iter->second = iter->second->next;
        delete old;
    }
    else
    {
        delete iter->second;
        vars.erase(iter);
    }
}

variable* vardict::getvar(std::string name)
{
    std::map<std::string, variable*>::iterator iter = vars.find(name);
    if (iter == vars.end())
        return 0;
    else
        return iter->second;
}

bool vardict::exists(std::string name)
{
    return vars.find(name) != vars.end();
}


vardict::vardict()
{
    typesizes[type_int] = 1;
    typesizes[type_pointer] = 2;
    first_available_space = 0;
    for (int i = 0; i < (HEAP_TOP - HEAP_BOTTOM); i++)
        memory.push_back(0);
}


//////////////////////// Linker defs /////////////////////////////


linker::linker()
{
    defined_funcs["nfc"] = 0;
    defined_funcs["nfc4"] = 0;  // hardcoded funcs

    index = 0;
    buffer.reserve(ROM_SIZE);
}

void linker::savelabel(std::string name, uint16_t address)
{
    valtable[name + "_HI"] = address >> 8;
    valtable[name + "_LO"] = address & 0xff;
}

// NB: index refers to the location _about_ to be written. (I.e. next location)
void linker::write8(linkval val)
{
    if (index >= ROM_SIZE)
        throw(error("Error: program too big!"));
    buffer.push_back(val);
    index++;
}

void linker::write16(linkval val)
{
    if (val.type == lv_literal)
    {
        write8(val.literal >> 8);           // big endian!
        write8(val.literal & 0xff);
    }
    else
    {
        write8(linkval(val.sym + "_HI"));
        write8(linkval(val.sym + "_LO"));
    }
}

void linker::padto8bytes()
{
    while ((buffer.size() % 8) != 0)
        write8(0);
}

void linker::generate_nfc2(linkval x, linkval y)
{
    padto8bytes();
    uint16_t next = index + 8;
    write16(x);
    write16(y);
    write16(next);
    write16(next);
}

// clear temp; set temp to not(x);  invert temp and branch on result.
void linker::generatebranchifzero(linkval testloc, linkval dest)
{
    uint16_t temploc = vars.addvar("__iftemp", type_int) + HEAP_BOTTOM;
    generate_nfc2(temploc, getconstaddress(0xff));
    generate_nfc2(temploc, testloc);
    uint16_t next = index + 8;
    write16(temploc);
    write16(temploc);
    write16(next);
    write16(dest);
    vars.remove("__iftemp");
}

void linker::generatebranchalways(linkval dest)
{
    uint16_t temploc = vars.addvar("__temp", type_int) + HEAP_BOTTOM;
    padto8bytes();
    write16(temploc);
    write16(temploc);
    write16(dest);
    write16(dest);
}

void linker::add_object(object *obj)
{
    // check for collisions:
    for (std::set<std::string>::iterator iter = obj->defined_funcs.begin(); iter != obj->defined_funcs.end(); iter++)
    {
        if (defined_funcs.find(*iter) != defined_funcs.end())
        {
            std::stringstream ss;
            ss << "Error: multiple definitions of function \"" << *iter << "\"";
            throw(error(ss.str()));
        }
    }
    // add all the definitions to linker's symbol table:
    for (unsigned int i = 0; i < obj->tree->defs.size(); i++)
    {
        definition *def = obj->tree->defs[i];
        if (def->type != dt_funcdef || !((funcdef*)def)->defined)
        {
            throw(error("Error: linker only accepts defined functions as symbols, wtf are you doing."));
        }
        funcdef *fdef = (funcdef*)def;
        defined_funcs[fdef->name] = fdef;
        definitions.push_back(def);
    }
}

// keep a dict of static vars.
// start at main, pass through the statements. Link in all the hardcoded funcs.
// increment the current machine index as you go.
std::vector<char> linker::link()
{
    if (defined_funcs.find("main") == defined_funcs.end())
    {
        throw(error("Error: no definition of function main."));
    }
    valtable.clear();
    link(defined_funcs["main"]->body);

    return assemble();
}

void linker::link(block *blk)
{
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->names.size(); i++)
        {
            vars.addvar((*iter)->names[i], (*iter)->type);
        }
    }
    for (std::vector<statement*>::iterator iter = blk->statements.begin(); iter != blk->statements.end(); iter++)
    {
        link(*iter);
    }
    // TODO: make this a simple scope pop operation.
    for (std::vector<vardeclaration*>::iterator iter = blk->declarations.begin(); iter != blk->declarations.end(); iter++)
    {
        for (unsigned int i = 0; i < (*iter)->names.size(); i++)
        {
            vars.remove((*iter)->names[i]);
        }
    }
}

void linker::link(statement *stat)
{
    switch(stat->type)
    {
    case stat_call:
        link((funccall*)stat);
        break;
    case stat_goto:
        link((goto_stat*)stat);
        break;
    case stat_label:
        savelabel(((label*)stat)->name, index);
        break;
    case stat_if:
        link((if_stat*)stat);
        break;
    case stat_while:
        link((while_stat*)stat);
        break;
    default:
        throw(error("Error: linking unrecognized statement type"));
    }
}

void linker::link(funccall *call)
{
    if (defined_funcs.find(call->name) == defined_funcs.end())
    {
        std::stringstream ss;
        ss << "Error: no definition for function \"" << call->name << "\"";
        throw(error(ss.str()));
    }
    funcdef *def = defined_funcs[call->name];
    if (!def)   // it's a hardcoded function...
    {
        if (call->name == "nfc")
        {
            padto8bytes();
            write16(evaluate(call->args[0]));
            write16(evaluate(call->args[1]));
            uint16_t next = index + 4;
            write16(next);
            write16(next);
        }
        else if (call->name == "nfc4")
        {
            padto8bytes();
            write16(evaluate(call->args[0]));
            write16(evaluate(call->args[1]));
            write16(evaluate(call->args[2]));
            write16(evaluate(call->args[3]));
        }
    }
}

void linker::link(goto_stat *sgoto)
{
    uint16_t temploc = vars.addvar("temp", type_int) + HEAP_BOTTOM;
    write16(temploc);
    write16(temploc);
    linkval target = evaluate(sgoto->target);
    write16(target);
    write16(target);
    vars.remove("temp");
}


//  BRNZ L1
//  <if block>
//  BRA L2
//L1:
//  <else block
//L2:
void linker::link(if_stat* ifs)
{
    std::string elselabel = getlabel();
    std::string endlabel = getlabel();
    linkval testloc = linker::evaluate(ifs->expr);
    generatebranchifzero(testloc, linkval(elselabel));
    link(ifs->ifblock);
    if (ifs->elseblock)
    {
        // finished the if clause: nonconditional jump past else clause.
        generatebranchalways(linkval(endlabel));
        savelabel(elselabel, index);
        link(ifs->elseblock);
    }
    else
        savelabel(elselabel, index);
    savelabel(endlabel, index);
}

//L1:
//  <test>
//  BRZ L2
//  <body>
//  BRA L1
//L2:
void linker::link(while_stat *whiles)
{
    std::string toplabel = getlabel();
    std::string exitlabel = getlabel();
    savelabel(toplabel, index);
    generatebranchifzero(evaluate(whiles->expr), exitlabel);
    link(whiles->blk);
    // jump unconditionally to top:
    generatebranchalways(linkval(toplabel));
    savelabel(exitlabel, index);
}


// can return a literal value or a symbol: this is transparent to using functions.
// (think of a symbol as an "IOU" for an actual address: e.g. labels we don't know the address til we reach them.)
linkval linker::evaluate(expression *expr)
{
    if (expr->type == exp_number)
    {
        return expr->number;
    }
    else if (expr->type == exp_name)
    {
        if (vars.exists(expr->name))
        {
            variable *var = vars.getvar(expr->name);
            if (var->type == type_label)
                return expr->name;
            else
                return vars.getvar(expr->name)->offset + HEAP_BOTTOM;
        }
        else
        {
            std::stringstream ss;
            ss << "Error: unknown linker symbol \"" << expr->name << "\"";
            throw(error(ss.str()));
        }
    }
    else
    {
        throw(error("Error: linking unknown expression type"));
    }
}


std::vector<char> linker::assemble()
{
    // generate halt instruction:
    generatebranchalways(index);
    // assemble literals and symbols into program:
    std::vector<char> image;
    for (std::vector<linkval>::iterator iter = buffer.begin(); iter != buffer.end(); iter++)
    {
        if (iter->type == lv_literal)
        {
            image.push_back(iter->literal);
        }
        else
        {
            image.push_back(valtable[iter->sym]);
        }
    }
    // put constant tables into last kilobyte.
    while (image.size() < (unsigned)INCREMENT_START)
        image.push_back(0);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i + 1) & 0xff);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i - 1) & 0xff);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i << 1) & 0xff);
    for (unsigned int i = 0; i <= 255; i++)
        image.push_back((i >> 1) & 0xff);
    return image;
}

